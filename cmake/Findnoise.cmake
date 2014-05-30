# Locate noise library

# This module defines the following variables:
# NOISE_LIBRARY, the name of the library;
# NOISE_INCLUDE_DIR, where to find glfw include files.
# NOISE_FOUND, true if both the NOISE_LIBRARY and NOISE_INCLUDE_DIR have been found.

#Search for the include file...
FIND_PATH(NOISE_INCLUDE_DIRS AntTweakBar.h DOC "Path to libnoise include directory."
  HINTS
  $ENV{NOISE_ROOT}
  PATH_SUFFIX include #For finding the include file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/include/
  /usr/local/include/
  ${NOISE_ROOT_DIR}/include/ # added by ptr
  ${CMAKE_SOURCE_DIR}/extlibs/headers
)

SET(NOISE_LIB_NAMES libnoise.dylib libnoise.a libnoise libnoise.dll libnoise.lib)

FIND_LIBRARY(NOISE_LIBRARY DOC "Absolute path to libnoise library."
  NAMES ${NOISE_LIB_NAMES}
  HINTS
  $ENV{NOISE_ROOT}
  PATH_SUFFIXES lib/win32 #For finding the library file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/local/lib
  /usr/lib
  ${NOISE_ROOT_DIR}/lib-msvc100/release # added by ptr
)

IF(NOISE_LIBRARY AND NOISE_INCLUDE_DIRS)
  SET(NOISE_FOUND TRUE)
  message(STATUS "Found libnoise: ${NOISE_LIBRARY}")
ELSE()
  message(STATUS "libnoise NOT found!")
    return()
ENDIF(NOISE_LIBRARY AND NOISE_INCLUDE_DIRS)

