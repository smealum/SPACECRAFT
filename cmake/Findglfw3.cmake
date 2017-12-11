# Locate the glfw library
# This module defines the following variables:
# GLFW_LIBRARY, the name of the library;
# GLFW_INCLUDE_DIR, where to find glfw include files.
# GLFW_FOUND, true if both the GLFW_LIBRARY and GLFW_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# GLFW_ROOT which points to the root of the glfw library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw library:
#
# FIND_PACKAGE (glfw REQUIRED)
# INCLUDE_DIRECTORIES (${GLFW_INCLUDE_DIRS})
# TARGET_LINK_LIBRARIES (executable ${GLFW_LIBRARIES})
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.

#Search for the include file...
FIND_PATH(GLFW_INCLUDE_DIRS GLFW/glfw3.h DOC "Path to GLFW include directory."
  HINTS
  $ENV{GLFW_ROOT}
  PATH_SUFFIX include #For finding the include file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/include/
  /usr/local/include/
  # By default headers are under GLFW subfolder
  /usr/include/GLFW
  /usr/local/include/GLFW
  ${GLFW_ROOT_DIR}/include/ # added by ptr
  ${CMAKE_SOURCE_DIR}/extlibs/headers
)

SET(GLFW_LIB_NAMES libglfw3.a glfw3.lib glfw3)

FIND_LIBRARY(GLFW_LIBRARIES DOC "Absolute path to GLFW library."
  NAMES ${GLFW_LIB_NAMES}
  HINTS
  $ENV{GLFW_ROOT}
  PATH_SUFFIXES lib/win32 #For finding the library file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/local/lib
  /usr/lib
  ${GLFW_ROOT_DIR}/lib-msvc100/release # added by ptr
)

IF( APPLE )
    find_library(IOKIT NAMES IOKit)
    #find_library(OpenGL NAMES OpenGL)
    find_library(COREVIDEO NAMES CoreVideo)
    find_library(COCOA NAMES Cocoa)
    find_library(pthread NAMES pthread)
    SET(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${IOKIT} ${COREVIDEO} ${COCOA} ${pthread})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_library(x11 NAMES X11)
  find_library(xrandr NAMES Xrandr)
  find_library(xi NAMES Xi)
  find_library(xxf86vm NAMES Xxf86vm)
  find_library(pthread NAMES pthread)
  SET(GLFW_LIBRARIES ${GLFW_LIBRARIES} ${x11} ${xrandr} ${xi} ${xxf86vm} ${pthread})
endif()

IF(GLFW_LIBRARIES AND GLFW_INCLUDE_DIRS)
  SET(GLFW_FOUND TRUE)
  message(STATUS "Found GLFW3: ${GLFW_LIBRARIES}")
ELSE()
  message(STATUS "GLFW3 NOT found!")
ENDIF(GLFW_LIBRARIES AND GLFW_INCLUDE_DIRS)

#if(GLFW_FOUND)
#  MARK_AS_ADVANCED(GLFW_INCLUDE_DIRS GLFW_LIBRARIES)
#endif(GLFW_FOUND)
