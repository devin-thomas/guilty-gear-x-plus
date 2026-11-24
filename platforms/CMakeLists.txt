# The native port of Guilty Gear X Plus (PORT-002: the headless Core host;
# PORT-003: the EE-FPU Model).
#
#   cmake -S platforms -B build -G Ninja -DGGX_VERSION=KR    (or JP)
#   cmake --build build --target port
#
# The game C in src/ggxeuc is compiled for wasm32 (the EE's 32-bit pointers
# and data layout), linked with wasm-ld, moved to the PS2's own addresses by
# tools/wasm_place.py, turned into C by wasm2c, and compiled into the host
# with the shims. platforms/README.md explains why and lists what each step
# needs. The matching build (build.py) never sees any of this.
cmake_minimum_required(VERSION 3.20)
project(ggx_port LANGUAGES C)

set(GGX_VERSION "KR" CACHE STRING "Release to build: KR (SLPM-645.25) or JP (SLPS-201.37)")
set_property(CACHE GGX_VERSION PROPERTY STRINGS KR JP)
if(NOT GGX_VERSION MATCHES "^(KR|JP)$")
  message(FATAL_ERROR "GGX_VERSION must be KR or JP, not '${GGX_VERSION}'")
endif()
string(TOLOWER "${GGX_VERSION}" GGX_TARGET)
option(GGX_WERROR "Treat warnings as errors" ON)
option(GGX_SYSTEM_WABT "Use the wasm2c on PATH and its runtime instead of building the pinned wabt" OFF)
option(GGX_FPU_IEEE "Developer check: the EE-FPU Model's interface with host IEEE floats (tests/fpu/ieee)" OFF)
option(GGX_STATE_SCAN "The Core also marks every page it reads, for ggx-state-test --scan (slower)" OFF)

get_filename_component(DECOMP "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(PLAT "${CMAKE_CURRENT_SOURCE_DIR}")
set(GEN "${CMAKE_CURRENT_BINARY_DIR}/gen")
find_package(Python3 REQUIRED COMPONENTS Interpreter)

include(cmake/CoreToolchain.cmake)
include(cmake/Wabt.cmake)

# ---------------------------------------------------------------------------
# The Simulation Core module (wasm32)
# ---------------------------------------------------------------------------
set(SYMBOL_MAP "${PLAT}/common/core/symbols/${GGX_TARGET}.txt")
# every static by TU, from the KR disc's stabs (tools/statics.py): wasm_place.py places them
set(STATICS_MAP "${PLAT}/common/core/symbols/statics_${GGX_TARGET}.txt")
set(CORE_FLAGS
  --target=wasm32 -mcpu=mvp -mmutable-globals -msign-ext -mnontrapping-fptoint -mbulk-memory
  -std=gnu89 -O2 -ffreestanding -fno-builtin -ffunction-sections -fdata-sections
  # the EE's integer behaviour, with no UB for the optimiser to exploit
  -fwrapv -fno-strict-aliasing -fno-delete-null-pointer-checks
  # uninitialised locals read as zero on every host, not as leftovers
  -ftrivial-auto-var-init=zero
  # a*b+c is two EE operations, never one fused one
  -ffp-contract=off
  -DVERSION_${GGX_VERSION} -DGGX_PORT -DSPLAT
  -I${DECOMP}/include -I${PLAT}/common/core/include)
if(GGX_WERROR)
  list(APPEND CORE_FLAGS -Werror)
endif()
# Game C is matched for EE-GCC 2.9 and written in its dialect. These
# warnings flag that dialect (K&R prototypes, char subscripts, pointer and
# integer mixing that is exact on a 32-bit target), not port bugs; real ABI
# mismatches are caught by tools/abi_check.py and wasm-ld instead.
set(GAME_FLAGS
  -DGGX_DECLARE_TU_STATICS "-Dlong=long long" -include port/game_prelude.h
  -Wall -Werror=implicit-function-declaration
  -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-incompatible-pointer-types-discards-qualifiers
  -Wno-deprecated-non-prototype -Wno-char-subscripts -Wno-unused-function -Wno-unused-variable
  -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-unused-label -Wno-unused-value
  -Wno-missing-braces -Wno-pointer-sign -Wno-bitwise-instead-of-logical -Wno-self-assign
  -Wno-pointer-bool-conversion -Wno-macro-redefined
  -Wno-unterminated-string-initialization -Wno-unknown-warning-option -Wno-sign-compare)
set(PORT_FLAGS -Wall -Wextra)

# PORT-015 (SPEC section 4): sanitizer builds, for CI's checks, never shipped.
#   undefined: the Core's game C with UBSan's checks compiled into core.wasm in trap mode (no runtime in the
#     wasm: undefined behaviour becomes a wasm trap, which the host reports with the function's name), and
#     UBSan with its runtime on everything native (the host, the shims, wasm2c's C of the Core);
#   memory: MemorySanitizer on everything native (clang on Linux); core.wasm is the usual one;
#   address: AddressSanitizer on everything native.
# A sanitizer build's core.wasm (with undefined) differs from core-wasm.sha256's, and its State Hashes can differ
# where the Core's own stack is hashed; the checks compare a build with itself.
set(GGX_SANITIZE "" CACHE STRING "A sanitizer build: undefined, memory or address (empty: none)")
if(GGX_SANITIZE)
  if(NOT GGX_SANITIZE MATCHES "^(undefined|memory|address)$")
    message(FATAL_ERROR "GGX_SANITIZE is undefined, memory or address, not ${GGX_SANITIZE}")
  endif()
  if(GGX_SANITIZE STREQUAL "undefined")
    # -fwrapv already defines signed overflow as the EE's wrap; every other check traps
    # each check its own trap (no-merge), so a trap's line in wasm2c's C says which check it was
    list(APPEND CORE_FLAGS -fsanitize=undefined -fsanitize-trap=undefined -fno-sanitize-merge=undefined)
    set(_san -fsanitize=undefined -fno-sanitize-recover=undefined)
  elseif(GGX_SANITIZE STREQUAL "memory")
    set(_san -fsanitize=memory -fsanitize-memory-track-origins=2)
  else()
    set(_san -fsanitize=address -fno-sanitize-recover=address)
  endif()
  # C only: wabt, the build's own C++ tool (wasm2c), stays as it is; MemorySanitizer would need an instrumented C++
  # library for it, and nothing it does runs in the port
  # line tables only: wasm2c's C of the Core is large, and full debug info with MSan's origins needs gigabytes per file
  add_compile_options("$<$<COMPILE_LANGUAGE:C>:${_san};-fno-omit-frame-pointer;-gline-tables-only>")
  add_link_options("$<$<LINK_LANGUAGE:C>:${_san}>")
endif()

file(GLOB GAME_TUS CONFIGURE_DEPENDS "${DECOMP}/src/ggxeuc/*.c")
# matched C for the C library parts the game uses: newlib's malloc, rand and
# errno, fdlibm, and libgcc's software floating point (fp-bit), which the EE
# runs for every double operation (common/fpu/ee_softfp.c)
file(GLOB SDK_TUS CONFIGURE_DEPENDS "${DECOMP}/src/sdk/libm/*.c")
foreach(n malloc mallocr mallocr_calloc mallocr_free sbrkr rand mlock errno)
  list(APPEND SDK_TUS "${DECOMP}/src/sdk/newlib/${n}.c")
endforeach()
foreach(n dp_bit fp_bit _floatdisf)
  list(APPEND SDK_TUS "${DECOMP}/src/sdk/gcc/${n}.c")
endforeach()
set(PORT_TOOLS ${PLAT}/tools/port_tu.py ${DECOMP}/scripts/csplit.py ${DECOMP}/scripts/sdkprofile.py)

# Every Core TU but the EE-FPU Model's own is compiled in two steps with
# tools/fpu_lower.py between them: clang's front end alone, to LLVM IR; the
# rewrite, which routes every floating-point operation through the EE-FPU
# Model or the EE's software floating point (PORT-003), before the optimiser
# can change any of them; then the optimiser and the wasm back end.
set(CORE_CODEGEN_FLAGS
  --target=wasm32 -mcpu=mvp -mmutable-globals -msign-ext -mnontrapping-fptoint -mbulk-memory
  -O2 -ffunction-sections -fdata-sections)
if(GGX_WERROR)
  list(APPEND CORE_CODEGEN_FLAGS -Werror)
endif()

set(CORE_OBJS)
# core_compile(<name> <source> <flags...>): front end, fpu_lower.py, back end
function(core_compile name src)
  set(ll "${GEN}/core/ir/${name}.ll")
  set(fpu "${GEN}/core/ir/${name}.fpu.ll")
  set(obj "${GEN}/core/obj/${name}.o")
  add_custom_command(OUTPUT "${ll}"
    COMMAND ${GGX_CORE_CC} ${CORE_FLAGS} ${ARGN} -Xclang -disable-llvm-passes -S -emit-llvm
            -MD -MF ${ll}.d ${src} -o ${ll}
    DEPENDS "${src}" DEPFILE "${ll}.d" VERBATIM
    COMMENT "Core ${name}")
  add_custom_command(OUTPUT "${fpu}" "${GEN}/core/ir/${name}.fpu.json"
    COMMAND Python3::Interpreter ${PLAT}/tools/fpu_lower.py ${ll} ${fpu} --report ${GEN}/core/ir/${name}.fpu.json
    DEPENDS "${ll}" ${PLAT}/tools/fpu_lower.py VERBATIM)
  add_custom_command(OUTPUT "${obj}"
    COMMAND ${GGX_CORE_CC} ${CORE_CODEGEN_FLAGS} -c ${fpu} -o ${obj}
    DEPENDS "${fpu}" VERBATIM)
  set(CORE_OBJS ${CORE_OBJS} "${obj}" PARENT_SCOPE)
endfunction()

# core_object(<tu name> <source> [LONG64] <flags...>): port_tu.py, then core_compile
function(core_object tu src)
  string(REPLACE "/" "_" stem "${tu}")
  set(prep "${GEN}/core/tu/${stem}.c")
  set(tu_opts)
  if(ARGV2 STREQUAL "LONG64")
    set(tu_opts --long64)
    list(REMOVE_AT ARGN 0)
  endif()
  add_custom_command(OUTPUT "${prep}"
    COMMAND Python3::Interpreter ${PLAT}/tools/port_tu.py ${tu} ${src} ${prep} --target ${GGX_TARGET} ${tu_opts}
    DEPENDS "${src}" ${PORT_TOOLS} VERBATIM)
  core_compile(${stem} "${prep}" ${ARGN})
  set(CORE_OBJS ${CORE_OBJS} PARENT_SCOPE)
endfunction()

foreach(src IN LISTS GAME_TUS)
  get_filename_component(n "${src}" NAME_WE)
  core_object(ggxeuc/${n} "${src}" ${GAME_FLAGS} -DGGX_TU_${n})
endforeach()
foreach(src IN LISTS SDK_TUS)
  get_filename_component(n "${src}" NAME_WE)
  get_filename_component(dir "${src}" DIRECTORY)
  get_filename_component(lib "${dir}" NAME)
  # fdlibm sets a double one word at a time (SET_LOW_WORD on a fresh local)
  set(sdk_flags ${GAME_FLAGS} -Wno-uninitialized -Wno-dangling-else -I${dir})
  if(lib STREQUAL "gcc")
    # libgcc writes `long long` as well as the EE's 64-bit `long`; fp-bit's
    # matched C returns some values through a register it never names
    # (common/fpu/ee_softfp.c doesn't call those)
    list(REMOVE_ITEM sdk_flags "-Dlong=long long")
    core_object(sdk/${lib}/${n} "${src}" LONG64 ${sdk_flags} -Wno-return-type)
  else()
    core_object(sdk/${lib}/${n} "${src}" ${sdk_flags})
  endif()
endforeach()

# port code in the Core, compiled as written
function(port_object name src)
  core_compile(port_${name} "${src}" ${ARGN})
  set(CORE_OBJS ${CORE_OBJS} PARENT_SCOPE)
endfunction()
# the EE-FPU Model and the software floating point wrappers: integer code,
# the only Core code not rewritten (fpu_check.py confirms it has no floating
# point)
set(FPU_DIR "${PLAT}/common/fpu")
set(FPU_CHECK COMMAND Python3::Interpreter ${PLAT}/tools/fpu_check.py ${GEN}/core/core.linked.wasm)
if(GGX_FPU_IEEE)
  set(FPU_DIR "${PLAT}/tests/fpu/ieee")
  set(FPU_CHECK)
endif()
# a local checkout may substitute its own model of the same interface
# (GGX_EE_FPU_MODEL, a path to a whole replacement for ee_fpu.c)
include(cmake/Local.cmake OPTIONAL)
foreach(n ee_fpu ee_softfp)
  set(obj "${GEN}/core/obj/port_${n}.o")
  set(src "${FPU_DIR}/${n}.c")
  if(n STREQUAL "ee_fpu" AND GGX_EE_FPU_MODEL AND NOT GGX_FPU_IEEE)
    set(src "${GGX_EE_FPU_MODEL}")
  endif()
  add_custom_command(OUTPUT "${obj}"
    COMMAND ${GGX_CORE_CC} ${CORE_FLAGS} ${PORT_FLAGS} -I${PLAT}/common/fpu -MD -MF ${obj}.d -c ${src}
            -o ${obj}
    DEPENDS ${src} DEPFILE "${obj}.d" VERBATIM
    COMMENT "Core port/${n}")
  list(APPEND CORE_OBJS "${obj}")
endforeach()
port_object(abi_glue ${PLAT}/common/core/abi_glue.c ${PORT_FLAGS})
port_object(libc ${PLAT}/common/core/libc.c ${PORT_FLAGS})
port_object(game_loop ${PLAT}/common/core/game_loop.c ${PORT_FLAGS} "-Dlong=long long")
add_custom_command(OUTPUT "${GEN}/core/data_syms.c"
  COMMAND Python3::Interpreter ${PLAT}/tools/gen_data_syms.py ${SYMBOL_MAP} ${GEN}/core/data_syms.c
  DEPENDS ${SYMBOL_MAP} ${PLAT}/tools/gen_data_syms.py ${PLAT}/tools/symmap.py VERBATIM)
port_object(data_syms "${GEN}/core/data_syms.c" -w)

# one object per line for the Python tools, quoted for wasm-ld
string(REPLACE ";" "\n" objs_lines "${CORE_OBJS}")
file(GENERATE OUTPUT "${GEN}/core/objects.txt" CONTENT "${objs_lines}\n")
set(objs_quoted "")
foreach(o IN LISTS CORE_OBJS)
  string(APPEND objs_quoted "\"${o}\"\n")
endforeach()
file(GENERATE OUTPUT "${GEN}/core/objects.rsp" CONTENT "${objs_quoted}")

set(CORE_LDFLAGS
  --no-entry --emit-relocs --no-gc-sections --import-undefined --fatal-warnings
  --export=GameBoot --export=GameField
  # EE RAM 0x0-0x2000000 and the scratchpad 0x70000000-0x70004000 come
  # first; the module's own data and stack go above them
  --global-base=1879113728 ${GGX_WASM_LD_STACK_AFTER_DATA} -z stack-size=1048576
  --initial-memory=1881145344 --max-memory=1881145344)
add_custom_command(OUTPUT "${GEN}/core/core.linked.wasm"
  COMMAND Python3::Interpreter ${PLAT}/tools/abi_check.py @${GEN}/core/objects.txt
  COMMAND ${GGX_WASM_LD} ${CORE_LDFLAGS} -o ${GEN}/core/core.linked.wasm @${GEN}/core/objects.rsp
  ${FPU_CHECK}
  DEPENDS ${CORE_OBJS} ${PLAT}/tools/abi_check.py ${PLAT}/tools/fpu_check.py ${PLAT}/tools/wasmfile.py VERBATIM
  COMMENT "Linking the Core module")
add_custom_command(OUTPUT "${GEN}/core/core.wasm" "${GEN}/core/placement.txt"
  COMMAND Python3::Interpreter ${PLAT}/tools/wasm_place.py --symbols ${SYMBOL_MAP}
          --objects ${GEN}/core/objects.txt --report ${GEN}/core/placement.txt
          ${GEN}/core/core.linked.wasm ${GEN}/core/core.wasm
  DEPENDS "${GEN}/core/core.linked.wasm" ${SYMBOL_MAP} ${STATICS_MAP}
          ${PLAT}/tools/wasm_place.py ${PLAT}/tools/wasmfile.py ${PLAT}/tools/symmap.py
          ${PLAT}/tools/symmap.py VERBATIM
  COMMENT "Placing the Core at its PS2 addresses")

set(W2C_OUT "${GEN}/w2c/ggx_core.h" "${GEN}/w2c/ggx_core-impl.h")
foreach(i RANGE 3)
  list(APPEND W2C_OUT "${GEN}/w2c/ggx_core_${i}.c")
endforeach()
add_custom_command(OUTPUT ${W2C_OUT}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${GEN}/w2c
  COMMAND ${GGX_WASM2C} ${GEN}/core/core.wasm -n ggx --num-outputs=4 -o ${GEN}/w2c/ggx_core.c
  COMMAND Python3::Interpreter ${PLAT}/tools/w2c_patch.py ${GEN}/w2c
  DEPENDS "${GEN}/core/core.wasm" ${GGX_WASM2C_DEPENDS} ${PLAT}/tools/w2c_patch.py VERBATIM
  COMMENT "wasm2c")
add_custom_command(OUTPUT "${GEN}/host_syms.c"
  COMMAND Python3::Interpreter ${PLAT}/tools/gen_host_syms.py ${SYMBOL_MAP} ${STATICS_MAP} ${GEN}/host_syms.c
  DEPENDS ${SYMBOL_MAP} ${STATICS_MAP} ${PLAT}/tools/gen_host_syms.py ${PLAT}/tools/symmap.py VERBATIM)

# ---------------------------------------------------------------------------
# The native host
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# wasm2c's output and runtime: generated or third-party, so not held to our
# warning set. The Core must compute the same thing on every host, so no
# FMA contraction anywhere.
add_library(ggx_core STATIC ${W2C_OUT} ${GGX_WASM_RT_SOURCES})
target_include_directories(ggx_core PUBLIC "${GEN}/w2c" ${GGX_WASM_RT_INCLUDE} ${GGX_WASM_RT_DIR})
if(MSVC)
  target_compile_options(ggx_core PRIVATE /W0 /fp:strict)
else()
  target_compile_options(ggx_core PRIVATE -w -ffp-contract=off -fno-strict-aliasing)
endif()
if(GGX_WASM2C_DEPENDS)
  add_dependencies(ggx_core ${GGX_WASM2C_DEPENDS})
endif()
if(GGX_STATE_SCAN)
  target_compile_definitions(ggx_core PUBLIC GGX_STATE_SCAN)
endif()
if(UNIX)
  target_link_libraries(ggx_core PUBLIC m)
endif()

# The Player Disc: image readers, identification, extraction and the
# extracted asset store (PORT-007). Release-independent: either build
# extracts either disc.
include(cmake/Chdr.cmake)
add_library(ggx_assets STATIC
  common/assets/assets.c common/assets/fsys.c common/assets/image.c common/assets/iso9660.c
  common/assets/releases.c common/assets/sha256.c common/assets/store.c)
target_include_directories(ggx_assets PUBLIC common/assets)
target_link_libraries(ggx_assets PRIVATE chdr)
if(WIN32)
  target_link_libraries(ggx_assets PRIVATE shell32)
endif()

# Saves (PORT-012): the native save format, PS2 memory card images, and
# the card the libmc shim answers from
add_library(ggx_saves STATIC common/saves/save_file.c common/saves/ps2mc.c common/saves/mc_card.c)
target_include_directories(ggx_saves PUBLIC common/saves)
target_include_directories(ggx_saves SYSTEM PRIVATE third_party/xxhash)
target_link_libraries(ggx_saves PUBLIC ggx_assets)
add_executable(ggx-save common/saves/save_main.c)
target_link_libraries(ggx-save PRIVATE ggx_saves)
if(WIN32)
  target_link_libraries(ggx-save PRIVATE shell32)
endif()

# the Core host, its state (PORT-004) and the shims, shared by the programs
# (objects, not an archive: the Core and the shims call each other)
add_library(ggx_host OBJECT
  common/host/core_host.c common/host/core_state.c common/host/disc.c
  common/shims/shim_cdvd.c common/shims/shim_gs.c common/shims/shim_mc.c common/shims/shim_pad.c
  common/shims/shim_sif.c common/trace/trace.c common/input/pad.c common/input/macro.c common/input/confirm.c common/input/ggscript.c
  common/host/menu.c common/host/unlock.c common/host/session.c common/host/rollback.c common/host/replays.c
  "${GEN}/host_syms.c")
target_include_directories(ggx_host PUBLIC common/host common/trace common/input)
# xxHash (BSD 2-Clause, third_party/xxhash/LICENSE): the State Hash
target_include_directories(ggx_host SYSTEM PRIVATE third_party/xxhash)
target_compile_definitions(ggx_host PUBLIC VERSION_${GGX_VERSION})
target_link_libraries(ggx_host PUBLIC ggx_core ggx_assets ggx_saves chdr)
# compressed Lockstep Traces (.ggxt.zst) are read with the zstd decoder libchdr bundles
target_include_directories(ggx_host SYSTEM PRIVATE third_party/libchdr/deps/zstd-1.5.7)
if(WIN32)
  target_link_libraries(ggx_host PUBLIC shell32)
endif()

# PORT-011: the sound driver re-implemented (audio/): the command stream, SNDRV.IRX on an SPU2 model, and
# the layer that feeds it from a Core host; it reads the Core's memory and never writes it
add_library(ggx_audio STATIC audio/snd_stream.c audio/spu2.c audio/spu2_regs.c audio/sndrv.c audio/audio_host.c)
target_include_directories(ggx_audio PUBLIC audio)
target_include_directories(ggx_audio SYSTEM PRIVATE third_party/xxhash)
target_link_libraries(ggx_audio PUBLIC ggx_host)
add_executable(ggx-snd audio/snd_main.c)
target_link_libraries(ggx-snd PRIVATE ggx_audio)
# the SPU2 model against PCSX2's own SPU2, from a tap of its register writes and output (no disc)
add_executable(ggx-spu2-replay audio/spu2_replay_main.c audio/spu2_tap.c)
target_link_libraries(ggx-spu2-replay PRIVATE ggx_audio)
# the port's sound driver against the game's on PCSX2, at the SPU2 (two taps of the same command stream)
add_executable(ggx-spu2-compare audio/spu2_compare_main.c audio/spu2_tap.c)
target_link_libraries(ggx-spu2-compare PRIVATE ggx_audio)

add_executable(ggx-headless common/host/headless_main.c)
add_executable(ggx-state-test common/host/state_test.c)
# PORT-013: the player's replays (list, info)
add_executable(ggx-replays common/host/replays_main.c)
# PORT-013: two instances playing each other through rollback, checked against a third that never predicts
add_executable(ggx-rollback-test common/host/rollback_test.c)
target_include_directories(ggx-rollback-test SYSTEM PRIVATE third_party/xxhash)
# PORT-006: replays a Lockstep Trace recorded on the PS2
add_executable(ggx-replay common/host/replay_main.c)
target_include_directories(ggx-replay SYSTEM PRIVATE third_party/xxhash)
# PORT-010: searches inputs that play a mode to its end, for the completion runs
add_executable(ggx-solve common/host/solve_main.c)
foreach(t ggx_assets ggx_saves ggx-save ggx_host ggx_audio ggx-headless ggx-state-test ggx-rollback-test ggx-replays ggx-replay ggx-solve ggx-snd ggx-spu2-replay ggx-spu2-compare)
  if(MSVC)
    target_compile_options(${t} PRIVATE /W4 $<$<BOOL:${GGX_WERROR}>:/WX> /fp:strict)
  else()
    target_compile_options(${t} PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror> -ffp-contract=off)
  endif()
endforeach()
target_link_libraries(ggx-headless PRIVATE ggx_host ggx_audio)
target_link_libraries(ggx-state-test PRIVATE ggx_host)
target_link_libraries(ggx-rollback-test PRIVATE ggx_host ggx_audio ggx_dl)
target_link_libraries(ggx-replays PRIVATE ggx_host)
target_link_libraries(ggx-replay PRIVATE ggx_host ggx_audio)
target_link_libraries(ggx-solve PRIVATE ggx_host)

# ---------------------------------------------------------------------------
# Tests (ctest): the EE-FPU Model against PCSX2's interpreter, and the EE's
# software floating point, both compiled natively
# ---------------------------------------------------------------------------
enable_testing()
add_executable(ggx-fpu-test tests/fpu/fpu_test.c common/fpu/ee_fpu.c)
target_include_directories(ggx-fpu-test PRIVATE common/fpu)
add_test(NAME ee-fpu-vectors COMMAND ggx-fpu-test ${PLAT}/tests/fpu/ee_fpu_vectors.txt)
if(COMMAND ggx_local_tests)
  ggx_local_tests()
endif()
# the matched fp-bit as port_tu.py gives it to the Core (`long` spelled out)
add_executable(ggx-softfp-test tests/fpu/softfp_test.c common/fpu/ee_softfp.c common/fpu/ee_fpu.c
  "${GEN}/core/tu/sdk_gcc_dp_bit.c" "${GEN}/core/tu/sdk_gcc_fp_bit.c" "${GEN}/core/tu/sdk_gcc__floatdisf.c")
target_include_directories(ggx-softfp-test PRIVATE common/fpu "${DECOMP}/src/sdk/gcc")
# fp-bit's bit-fields mix types; MinGW would lay them out the Microsoft way
set(softfp_opts -w)
if(WIN32 AND NOT MSVC)
  list(APPEND softfp_opts -mno-ms-bitfields)
endif()
set_source_files_properties("${GEN}/core/tu/sdk_gcc_dp_bit.c" "${GEN}/core/tu/sdk_gcc_fp_bit.c"
  "${GEN}/core/tu/sdk_gcc__floatdisf.c" PROPERTIES COMPILE_OPTIONS "${softfp_opts}")
add_test(NAME ee-softfp COMMAND ggx-softfp-test)
foreach(t ggx-fpu-test ggx-softfp-test ${GGX_LOCAL_FPU_TESTS})
  if(MSVC)
    target_compile_options(${t} PRIVATE /W4 /fp:strict)
  else()
    target_compile_options(${t} PRIVATE -Wall -Wextra -ffp-contract=off)
  endif()
endforeach()

# PORT-010: the pad shim's bytes against every pad read in the Lockstep Trace corpus, and GGScript; no disc
add_executable(ggx-pad-test tests/input/pad_test.c)
target_link_libraries(ggx-pad-test PRIVATE ggx_host psi_core)
if(MSVC)
  target_compile_options(ggx-pad-test PRIVATE /W4)
else()
  target_compile_options(ggx-pad-test PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror>)
endif()
# the pad shim against a Lockstep Trace corpus, where a checkout has one (GGX_TRACE_DIR; not in this repository)
if(GGX_TRACE_DIR AND EXISTS "${GGX_TRACE_DIR}/scripts")
  file(GLOB GGX_CORPUS_TRACES "${GGX_TRACE_DIR}/*.ggxt.zst")
  add_test(NAME pad-shim COMMAND ggx-pad-test --scripts "${GGX_TRACE_DIR}/scripts" ${GGX_CORPUS_TRACES})
endif()

# PORT-011: the SPU2 model and the sound driver, on made-up data; no disc
add_executable(ggx-audio-test tests/audio/audio_test.c)
target_link_libraries(ggx-audio-test PRIVATE ggx_audio)
if(MSVC)
  target_compile_options(ggx-audio-test PRIVATE /W4)
else()
  target_compile_options(ggx-audio-test PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror>)
endif()
add_test(NAME audio COMMAND ggx-audio-test "${CMAKE_CURRENT_BINARY_DIR}")

# PORT-012: the save format, the export check and memory card images, with no disc
add_executable(ggx-save-test tests/saves/save_test.c)
target_link_libraries(ggx-save-test PRIVATE ggx_saves)
if(MSVC)
  target_compile_options(ggx-save-test PRIVATE /W4)
else()
  target_compile_options(ggx-save-test PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror>)
endif()
add_test(NAME saves COMMAND ggx-save-test "${CMAKE_CURRENT_BINARY_DIR}")

# PORT-013: the replay format and its refusals, with no disc
add_executable(ggx-replays-test tests/replays/replays_test.c)
target_link_libraries(ggx-replays-test PRIVATE ggx_host)
target_include_directories(ggx-replays-test SYSTEM PRIVATE third_party/xxhash)
if(MSVC)
  target_compile_options(ggx-replays-test PRIVATE /W4)
else()
  target_compile_options(ggx-replays-test PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror>)
endif()
add_test(NAME replays COMMAND ggx-replays-test "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(port ALL DEPENDS ggx-headless ggx-state-test ggx-rollback-test ggx-replay ggx-solve ggx-snd ggx-spu2-replay ggx-spu2-compare ggx-audio-test ggx-pad-test ggx-fpu-test
  ggx-softfp-test ${GGX_LOCAL_FPU_TESTS} ggx-save ggx-save-test ggx-replays ggx-replays-test)

# ---------------------------------------------------------------------------
# The Draw Layer renderer and ggx, the SDL3 host (PORT-008), when SDL3 is
# found (MSYS2 mingw-w64-ucrt-x86_64-sdl3, Debian libsdl3-dev, Homebrew sdl3).
# The shaders are precompiled in render/shaders/dl_shaders.h.
# ---------------------------------------------------------------------------
option(GGX_SDL "Build ggx, the SDL3 host with the SDL_GPU renderer, if SDL3 is found" ON)
option(GGX_DEV_BUILD "A developer build of ggx: Tab runs the Core unthrottled" OFF)
if(GGX_SDL)
  find_package(SDL3 3.2 CONFIG)
endif()

# game-kit (https://github.com/uppercut-labs/game-kit): the input layer (port-sdl-input: devices, players, bindings,
# SOCD, analog, remap capture, prompt letters) and the settings file (port-settings), each at a release tag. The
# port's own part is sdl/input.c (its buttons as data, the macros, the confirm swap). Offline: set
# -DFETCHCONTENT_SOURCE_DIR_PORT_SDL_INPUT and -DFETCHCONTENT_SOURCE_DIR_PORT_SETTINGS to a game-kit checkout.
# The headless programs build without SDL, so port-sdl-input is not required to find it here: ggx needs it.
include(FetchContent)
set(PSI_REQUIRE_SDL OFF)
FetchContent_Declare(port_settings
  GIT_REPOSITORY https://github.com/uppercut-labs/game-kit.git
  GIT_TAG        port-settings@0.1.0
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  packages/port-settings)
FetchContent_Declare(port_sdl_input
  GIT_REPOSITORY https://github.com/uppercut-labs/game-kit.git
  GIT_TAG        port-sdl-input@0.1.0
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  packages/port-sdl-input)
FetchContent_MakeAvailable(port_settings port_sdl_input)
# the renderer's input side, no SDL: the GIF stream, GS local memory, textures
add_library(ggx_dl STATIC render/gs_mem.c render/dl_tex.c render/dl_stream.c)
target_include_directories(ggx_dl PUBLIC render common/input)
# PORT-014: the English text layer (text/), drawn with stb_truetype
# (third_party/stb), and ggx-text, its proof sheets. The translation files
# and fonts are copied beside the programs, into text/.
add_library(ggx_text STATIC text/tl.c text/tl_draw.c text/tl_font.c text/tl_stb.c)
target_include_directories(ggx_text PUBLIC text)
target_include_directories(ggx_text SYSTEM PRIVATE third_party/stb)
target_link_libraries(ggx_text PUBLIC ggx_dl ggx_assets)
if(UNIX)
  # stb_truetype's acos, sqrt...: after ggx_text on the link line, wherever it lands
  target_link_libraries(ggx_text PUBLIC m)
endif()
set_source_files_properties(text/tl_stb.c PROPERTIES COMPILE_OPTIONS "$<IF:$<C_COMPILER_ID:MSVC>,/w,-w>")
add_executable(ggx-text text/text_main.c text/tl_host.c sdl/png_write.c)
target_include_directories(ggx-text PRIVATE sdl)
target_compile_definitions(ggx-text PRIVATE GGX_TEXT_SOURCE_DIR="${PLAT}/text")
target_link_libraries(ggx-text PRIVATE ggx_host ggx_text)
# copied on every build, so an edited translation file is picked up
add_custom_target(ggx-text-data ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLAT}/text/en" "${CMAKE_CURRENT_BINARY_DIR}/text/en"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLAT}/text/fonts" "${CMAKE_CURRENT_BINARY_DIR}/text/fonts")
add_dependencies(ggx-text ggx-text-data)
add_dependencies(port ggx-text)
# the GS Path as the stream interprets it (PORT-009): no GPU, no game data
add_executable(ggx-gs-path-test tests/render/gs_path_test.c)
target_link_libraries(ggx-gs-path-test PRIVATE ggx_dl)
add_test(NAME gs-path COMMAND ggx-gs-path-test)
# display pacing (sdl/pace.c) on made-up displays: no SDL, window or disc
add_executable(ggx-pace-test tests/pace/pace_test.c sdl/pace.c)
target_include_directories(ggx-pace-test PRIVATE sdl)
if(UNIX AND NOT APPLE)
  target_link_libraries(ggx-pace-test PRIVATE m)
endif()
add_test(NAME pace COMMAND ggx-pace-test)
add_dependencies(port ggx-pace-test)
add_dependencies(port ggx-gs-path-test)
# PORT-017: the button prompts (prompts/), letters from the game's HUD font
# in place of its PS2 symbols, and their test (synthetic pictures, no disc)
add_library(ggx_prompts STATIC prompts/pr.c)
target_include_directories(ggx_prompts PUBLIC prompts)
target_link_libraries(ggx_prompts PUBLIC ggx_text)
add_executable(ggx-prompts-test tests/prompts/pr_test.c)
target_link_libraries(ggx-prompts-test PRIVATE ggx_prompts)
add_test(NAME prompts COMMAND ggx-prompts-test)
add_dependencies(port ggx-prompts-test)

set(PORT_TARGETS ggx_dl ggx_text ggx-text ggx-gs-path-test ggx_prompts ggx-prompts-test ggx-pace-test)
if(SDL3_FOUND)
  # PORT-010: the input layer and the settings file
  add_executable(ggx sdl/ggx_main.c sdl/firstrun.c sdl/png_write.c sdl/input.c sdl/audio_out.c sdl/settings.c
    sdl/pace.c sdl/vblank.c render/dl_gpu.c text/tl_host.c)
  target_include_directories(ggx PRIVATE sdl)
  target_link_libraries(ggx PRIVATE ggx_host ggx_audio ggx_prompts ggx_text psi pset SDL3::SDL3)
  # the display's vblanks (sdl/vblank.c): the kernel's per-output wait (gdi32) or DXGI's, on Windows
  if(WIN32)
    target_link_libraries(ggx PRIVATE dxgi dxguid gdi32 user32)
    # a windowed program: no console window beside the game when started from Explorer or a shortcut
    # (SDL_main.h supplies WinMain); its messages still reach a pipe or file it is started with
    set_target_properties(ggx PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()
  if(GGX_DEV_BUILD)
    # the speed toggle (Tab) exists only in a developer build (SPEC section 5)
    target_compile_definitions(ggx PRIVATE GGX_DEV_BUILD)
  endif()
  add_dependencies(ggx ggx-text-data)
  if(WIN32)
    # SDL3.dll beside ggx.exe, so it runs from a plain Windows shell
    add_custom_command(TARGET ggx POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:ggx> $<TARGET_FILE_DIR:ggx>
      COMMAND_EXPAND_LISTS)
    # and every DLL those need in turn (SDL3.dll needs libiconv-2.dll), from the compiler's and SDL's directories,
    # so it starts from a shortcut too (PORT-018); tools/check_dlls.sh checks the result
    get_filename_component(GGX_CC_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    find_program(GGX_OBJDUMP NAMES objdump llvm-objdump HINTS "${GGX_CC_DIR}")
    set(GGX_SDL_DIR "")
    if(TARGET SDL3::SDL3-shared)
      set(GGX_SDL_DIR "$<TARGET_FILE_DIR:SDL3::SDL3-shared>")
    endif()
    if(GGX_OBJDUMP)
      add_custom_command(TARGET ggx POST_BUILD
        COMMAND ${CMAKE_COMMAND} -DEXE=$<TARGET_FILE:ggx> -DOBJDUMP=${GGX_OBJDUMP} -DDIR1=${GGX_CC_DIR}
          -DDIR2=${GGX_SDL_DIR} -P "${PLAT}/cmake/BundleDlls.cmake"
        VERBATIM)
    else()
      message(WARNING "no objdump: ggx.exe's DLLs beyond SDL3.dll are not copied beside it")
    endif()
  endif()
  list(APPEND PORT_TARGETS ggx)
  add_dependencies(port ggx)
  # the input layer with SDL's virtual controllers: no window, GPU or disc
  # PORT-019/020: the settings panel driven by pad presses, its neighbours stubbed (no window, disc or Core)
  add_executable(ggx-settings-test tests/settings/settings_test.c sdl/settings.c common/input/confirm.c)
  # core_host.h's headers, ggx_core.h (made by wasm2c) among them, without ggx_host's objects (the stubs stand in)
  target_include_directories(ggx-settings-test PRIVATE sdl prompts text render
    $<TARGET_PROPERTY:ggx_host,INTERFACE_INCLUDE_DIRECTORIES> $<TARGET_PROPERTY:ggx_core,INTERFACE_INCLUDE_DIRECTORIES>)
  add_dependencies(ggx-settings-test ggx_core)
  target_link_libraries(ggx-settings-test PRIVATE ggx_assets pset psi_core SDL3::SDL3)
  add_test(NAME settings COMMAND ggx-settings-test "${CMAKE_CURRENT_BINARY_DIR}")
  add_dependencies(port ggx-settings-test)
  add_executable(ggx-input-test tests/input/input_test.c sdl/input.c common/input/pad.c
    common/input/macro.c common/input/confirm.c)
  target_include_directories(ggx-input-test PRIVATE sdl common/input common/assets)
  target_link_libraries(ggx-input-test PRIVATE ggx_assets psi pset SDL3::SDL3)
  add_test(NAME input-layer COMMAND ggx-input-test "${CMAKE_CURRENT_BINARY_DIR}")
  list(APPEND PORT_TARGETS ggx-input-test)
  add_dependencies(port ggx-input-test)
elseif(GGX_SDL)
  message(STATUS "SDL3 not found: building ggx-headless only")
endif()
foreach(t ${PORT_TARGETS})
  if(MSVC)
    target_compile_options(${t} PRIVATE /W4 $<$<BOOL:${GGX_WERROR}>:/WX>)
  else()
    target_compile_options(${t} PRIVATE -Wall -Wextra $<$<BOOL:${GGX_WERROR}>:-Werror>)
  endif()
endforeach()
