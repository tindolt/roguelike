
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was libtcodConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

find_package(SDL3 QUIET)
find_package(ZLIB QUIET)
find_package(lodepng-c QUIET)
find_package(utf8proc QUIET)
find_package(unofficial-utf8proc QUIET)

set_and_check(LIBTCOD_INCLUDE_DIR "include")

include(${CMAKE_CURRENT_LIST_DIR}/libtcodTargets.cmake)

set(LIBTCOD_LIBRARY libtcod::libtcod)

check_required_components(libtcod)
