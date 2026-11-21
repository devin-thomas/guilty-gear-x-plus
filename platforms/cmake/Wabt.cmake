# wasm2c and its runtime. By default wabt is built from its pinned release,
# so every OS turns the Core into the same C with the same runtime. With
# GGX_SYSTEM_WABT=ON the wasm2c on PATH is used, with the runtime installed
# beside it (share/wabt/wasm2c and include/).
set(GGX_WABT_VERSION 1.0.41)
set(GGX_WABT_SHA256 ca9e69cc1de13b4633a3c74fd697319303b21108529d4f10960af4e1f4a65893)

set(GGX_WASM2C_DEPENDS)
if(GGX_SYSTEM_WABT)
  find_program(GGX_WASM2C wasm2c REQUIRED)
  get_filename_component(_prefix "${GGX_WASM2C}" DIRECTORY)
  get_filename_component(_prefix "${_prefix}" DIRECTORY)
  find_path(GGX_WASM_RT_DIR wasm-rt-impl.c HINTS "${_prefix}/share/wabt/wasm2c" "${_prefix}/share/wasm2c" REQUIRED)
  find_path(GGX_WASM_RT_INCLUDE wasm-rt.h HINTS "${_prefix}/include" "${GGX_WASM_RT_DIR}" REQUIRED)
  execute_process(COMMAND ${GGX_WASM2C} --version OUTPUT_VARIABLE _v OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "wasm2c: ${GGX_WASM2C} (${_v}, from PATH)")
else()
  include(ExternalProject)
  set(_wabt "${CMAKE_CURRENT_BINARY_DIR}/wabt")
  # third-party code: its warnings are not ours to fix
  if(MSVC)
    set(_quiet /w)
  else()
    set(_quiet -w)
  endif()
  ExternalProject_Add(wabt
    URL https://github.com/WebAssembly/wabt/releases/download/${GGX_WABT_VERSION}/wabt-${GGX_WABT_VERSION}.tar.xz
    URL_HASH SHA256=${GGX_WABT_SHA256}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    PREFIX "${_wabt}"
    SOURCE_DIR "${_wabt}/source"
    BINARY_DIR "${_wabt}/build"
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF -DUSE_INTERNAL_SHA256=ON
               -DWITH_EXCEPTIONS=OFF -DCMAKE_C_FLAGS=${_quiet} -DCMAKE_CXX_FLAGS=${_quiet}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target wasm2c
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${_wabt}/build/wasm2c${CMAKE_EXECUTABLE_SUFFIX}"
                     "${_wabt}/source/wasm2c/wasm-rt-impl.c" "${_wabt}/source/wasm2c/wasm-rt-mem-impl.c"
                     "${_wabt}/source/wasm2c/wasm-rt-exceptions-impl.c")
  set(GGX_WASM2C "${_wabt}/build/wasm2c${CMAKE_EXECUTABLE_SUFFIX}")
  set(GGX_WASM_RT_DIR "${_wabt}/source/wasm2c")
  set(GGX_WASM_RT_INCLUDE "${_wabt}/source/wasm2c")
  set(GGX_WASM2C_DEPENDS wabt)
  message(STATUS "wasm2c: wabt ${GGX_WABT_VERSION}, built from source")
endif()

set(GGX_WASM_RT_SOURCES
  "${GGX_WASM_RT_DIR}/wasm-rt-impl.c" "${GGX_WASM_RT_DIR}/wasm-rt-mem-impl.c"
  "${GGX_WASM_RT_DIR}/wasm-rt-exceptions-impl.c")
set_source_files_properties(${GGX_WASM_RT_SOURCES} PROPERTIES GENERATED TRUE)
