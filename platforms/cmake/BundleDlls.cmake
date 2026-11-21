# Copies every DLL a Windows program needs, and they need in turn, from the toolchain's and libraries' directories
# into the program's own (PORT-018), so ggx.exe starts from a shortcut, not only from MSYS2's shell where they are on
# PATH. SDL3.dll, for one, needs libiconv-2.dll.
#
#   cmake -DEXE=<ggx.exe> -DOBJDUMP=<objdump> -DDIR1=<dir> [-DDIR2=<dir>] -P BundleDlls.cmake
#
# A DLL found in neither directory is taken to be Windows' own; tools/check_dlls.sh checks that for the result.

get_filename_component(dest "${EXE}" DIRECTORY)
set(DIRS "${DIR1}" "${DIR2}")
set(seen "")

function(bundle file)
  execute_process(COMMAND "${OBJDUMP}" -p "${file}" OUTPUT_VARIABLE out RESULT_VARIABLE rc ERROR_QUIET)
  if(NOT rc EQUAL 0)
    message(FATAL_ERROR "BundleDlls: ${OBJDUMP} -p ${file} failed")
  endif()
  string(REGEX MATCHALL "DLL Name: [^\r\n]+" names "${out}")
  foreach(n ${names})
    string(REGEX REPLACE "DLL Name: " "" dll "${n}")
    string(STRIP "${dll}" dll)
    string(TOLOWER "${dll}" key)
    if(key IN_LIST seen)
      continue()
    endif()
    list(APPEND seen "${key}")
    set(seen "${seen}" PARENT_SCOPE)
    set(found "")
    if(EXISTS "${dest}/${dll}")
      set(found "${dest}/${dll}")
    else()
      foreach(d ${DIRS})
        if(EXISTS "${d}/${dll}")
          file(COPY "${d}/${dll}" DESTINATION "${dest}")
          message(STATUS "BundleDlls: ${dll} from ${d}")
          set(found "${dest}/${dll}")
          break()
        endif()
      endforeach()
    endif()
    if(found)
      bundle("${found}")
      set(seen "${seen}" PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

bundle("${EXE}")
