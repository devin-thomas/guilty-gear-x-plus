# The compiler and linker for the Core module: clang with the WebAssembly
# target and LLVM's wasm-ld, from one pinned release on every host. Which
# compiler builds the Core decides its wasm stack frames, and the game keeps
# stack addresses in EE RAM (pause_training's hw[].p points at a local
# array), so two compilers give two different State Hashes. The first
# configure downloads wasi-sdk for the host, checks it against the hash
# below and unpacks it into GGX_TOOLS_DIR; every OS then builds the same
# core.wasm, and wasm2c and the host compiler can't change linear memory.
# GGX_SYSTEM_CORE_CC=ON uses the clang and wasm-ld found on the system
# instead, for development only: its Core matches no other host's.
set(GGX_WASI_SDK_VERSION 34.0)
# SHA-256 of each host's archive, from the release's asset digests
set(_wasi_sdk_sha256_x86_64-windows cccb5c323a9b34f0349a9b09e8804a0a7632c68c3310f4b5f437ed57d7e71d8f)
set(_wasi_sdk_sha256_arm64-windows  45e1c71f3e965621e7b98ebe1d37b0e4b1f77f3e8072113ffb4534e67b1a4b7c)
set(_wasi_sdk_sha256_x86_64-linux   b761e3a0721dbae9c09a0059e5fdb2bf917d1b4a8a7b430fb3b5aafb0984b2c4)
set(_wasi_sdk_sha256_arm64-linux    f7e243dff54d60bcc576e94d6166b69f410f2500ae4a9ceef34315be10e77971)
set(_wasi_sdk_sha256_x86_64-macos   87d27fa8adc68dee59bfbf2e22a6d34ef717c34d6bf1d8af2a56fc929d9ce0eb)
set(_wasi_sdk_sha256_arm64-macos    9c59398106b417f8f14913380fdf0097a8cc0ff4af9eb3ce0065a859e88d49e9)

option(GGX_SYSTEM_CORE_CC "Build the Core with the system's clang and wasm-ld instead of the pinned wasi-sdk" OFF)
set(GGX_TOOLS_DIR "$ENV{GGX_TOOLS_DIR}" CACHE PATH
    "Where the pinned wasi-sdk is unpacked (shared by build directories; default: <build>/tools)")
if(NOT GGX_TOOLS_DIR)
  set(GGX_TOOLS_DIR "${CMAKE_CURRENT_BINARY_DIR}/tools")
endif()
file(TO_CMAKE_PATH "${GGX_TOOLS_DIR}" GGX_TOOLS_DIR)

if(GGX_SYSTEM_CORE_CC)
  set(_llvm_versions 22 21 20 19 18)
  set(_clang_names clang)
  set(_ld_names wasm-ld)
  foreach(v IN LISTS _llvm_versions)
    list(APPEND _clang_names clang-${v})
    list(APPEND _ld_names wasm-ld-${v})
  endforeach()
  set(_hints)
  if(APPLE)
    list(APPEND _hints /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin /opt/homebrew/opt/lld/bin /usr/local/opt/lld/bin)
  endif()
  find_program(GGX_CORE_CC NAMES ${_clang_names} HINTS ${_hints} DOC "clang with the wasm32 target, for the Core")
  find_program(GGX_WASM_LD NAMES ${_ld_names} HINTS ${_hints} DOC "LLVM wasm-ld, for the Core")
  if(NOT GGX_CORE_CC OR NOT GGX_WASM_LD)
    message(FATAL_ERROR "The Core needs clang with the wasm32 target and wasm-ld (LLVM 18 or later). "
                        "Set GGX_CORE_CC and GGX_WASM_LD.")
  endif()
  message(WARNING "GGX_SYSTEM_CORE_CC: this Core's State Hashes match only Cores built by the same compiler")
else()
  string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}" _cpu)
  if(_cpu MATCHES "^(x86_64|amd64)$")
    set(_cpu x86_64)
  elseif(_cpu MATCHES "^(arm64|aarch64)$")
    set(_cpu arm64)
  endif()
  if(CMAKE_HOST_WIN32)
    set(_os windows)
  elseif(CMAKE_HOST_APPLE)
    set(_os macos)
  else()
    set(_os linux)
  endif()
  set(_host "${_cpu}-${_os}")
  if(NOT DEFINED _wasi_sdk_sha256_${_host})
    message(FATAL_ERROR "No pinned wasi-sdk for this host (${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_PROCESSOR}); "
                        "-DGGX_SYSTEM_CORE_CC=ON builds with the system's clang, matching no other host")
  endif()
  set(_name "wasi-sdk-${GGX_WASI_SDK_VERSION}-${_host}")
  set(_sdk "${GGX_TOOLS_DIR}/${_name}")
  if(NOT EXISTS "${_sdk}/.ggx-sha256-${_wasi_sdk_sha256_${_host}}")
    string(REGEX REPLACE "\\..*" "" _major "${GGX_WASI_SDK_VERSION}")
    set(_archive "${GGX_TOOLS_DIR}/${_name}.tar.gz")
    message(STATUS "Downloading ${_name} for the Core (about 190 MB; 620 MB on Windows)")
    file(DOWNLOAD "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${_major}/${_name}.tar.gz"
         "${_archive}" EXPECTED_HASH SHA256=${_wasi_sdk_sha256_${_host}} STATUS _st)
    list(GET _st 0 _rc)
    if(NOT _rc EQUAL 0)
      file(REMOVE "${_archive}")
      message(FATAL_ERROR "Downloading ${_name} failed: ${_st}")
    endif()
    file(REMOVE_RECURSE "${_sdk}")
    file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${GGX_TOOLS_DIR}")
    file(REMOVE "${_archive}")
    if(NOT EXISTS "${_sdk}/bin")
      message(FATAL_ERROR "${_archive} did not unpack to ${_sdk}")
    endif()
    file(TOUCH "${_sdk}/.ggx-sha256-${_wasi_sdk_sha256_${_host}}")
  endif()
  set(GGX_CORE_CC "${_sdk}/bin/clang${CMAKE_HOST_EXECUTABLE_SUFFIX}" CACHE FILEPATH "" FORCE)
  set(GGX_WASM_LD "${_sdk}/bin/wasm-ld${CMAKE_HOST_EXECUTABLE_SUFFIX}" CACHE FILEPATH "" FORCE)
endif()

set(_probe "${CMAKE_CURRENT_BINARY_DIR}/core_probe")
file(MAKE_DIRECTORY "${_probe}")
file(WRITE "${_probe}/probe.c" "int ggx_probe(int x) { return x + 1; }\n")
execute_process(COMMAND ${GGX_CORE_CC} --target=wasm32 -c "${_probe}/probe.c" -o "${_probe}/probe.o"
                RESULT_VARIABLE _rc OUTPUT_QUIET ERROR_VARIABLE _err)
if(NOT _rc EQUAL 0)
  message(FATAL_ERROR "${GGX_CORE_CC} cannot compile for wasm32 (on macOS, Xcode's clang can't; "
                      "use Homebrew llvm):\n${_err}")
endif()

# Newer wasm-ld puts the stack at address 0 unless told otherwise, which
# would put it in EE RAM.
execute_process(COMMAND ${GGX_WASM_LD} --help OUTPUT_VARIABLE _ld_help ERROR_QUIET)
if(_ld_help MATCHES "--no-stack-first")
  set(GGX_WASM_LD_STACK_AFTER_DATA --no-stack-first)
else()
  set(GGX_WASM_LD_STACK_AFTER_DATA)
endif()
execute_process(COMMAND ${GGX_CORE_CC} --version OUTPUT_VARIABLE _ver ERROR_QUIET)
string(REGEX MATCH "version [0-9.]+" _ver "${_ver}")
message(STATUS "Core compiler: ${GGX_CORE_CC} (${_ver}), linker: ${GGX_WASM_LD}")
