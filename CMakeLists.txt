cmake_minimum_required(VERSION 3.9)
project(spacraft)

# Detect Web
if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  set(Web true)
else()
  set(Web false)
endif()
message(" * Build type : " ${CMAKE_BUILD_TYPE})
message(" * C compiler: ${CMAKE_C_COMPILER}")
message(" * C++ compiler: ${CMAKE_CXX_COMPILER}")
message(" * System: " ${CMAKE_SYSTEM_NAME} )
message(" * Use Web : " ${Web})

if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
  add_definitions(-DNDEBUG)
  #add_definitions(-DNTWBAR)
endif()

# C++ 14
if (Web)
  string(APPEND CMAKE_CXX_FLAGS " --std=c++14")
else()
  set (CMAKE_CXX_STANDARD 14)
endif()

# modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Enable compiler warning.
string(APPEND CMAKE_CXX_FLAGS " -Wall")
string(APPEND CMAKE_CXX_FLAGS " -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-reorder")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-parameter")

# Our code resides in ./src
include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE source_files ${PROJECT_SOURCE_DIR}/src/*)

# Put executable in the bin directory (,because we liked it.)
if (Web)
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin_web)
else()
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
endif()

# OS specifics -----------------------------------------------------------------

#------------
#    Web
#------------
if (Web)
  set(spacecraft_executable "spacecraft.js")
  include_directories(${CMAKE_SOURCE_DIR}/extlibs/headers/)
  list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/extlibs/libs-linux")

  # Emscripten options
  string(APPEND CMAKE_CXX_FLAGS " -s USE_GLFW=3 ")
  #string(APPEND CMAKE_CXX_FLAGS " -s NO_FILESYSTEM=1 ")
  string(APPEND CMAKE_CXX_FLAGS " -s DEMANGLE_SUPPORT=1")
  string(APPEND CMAKE_CXX_FLAGS " -s ALLOW_MEMORY_GROWTH=1")
  string(APPEND CMAKE_CXX_FLAGS " -s ASSERTIONS=1")
  string(APPEND CMAKE_CXX_FLAGS " -s USE_WEBGL2=1")
  string(APPEND CMAKE_CXX_FLAGS " -s FULL_ES3=1")

  # Allow the directories /shader and /data to be fetched by the web browser.
  file(GLOB shader_files "${CMAKE_SOURCE_DIR}/shader/**/*")
  file(GLOB data_files "${CMAKE_SOURCE_DIR}/data/*")
  set(imported_files ${shader_files} ${data_files})
  foreach(file ${imported_files})
    file(RELATIVE_PATH relative_file ${CMAKE_SOURCE_DIR} ${file})
    string(APPEND CMAKE_CXX_FLAGS " --preload-file ${file}@/${relative_file}")
  endforeach()

  # Do not compile glew (only for windows).
  list(REMOVE_ITEM source_files ${SRC}/glew/glew.c)

#------------
#   Linux
#------------
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(spacecraft_executable "spacecraft")
  list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/extlibs/libs-linux")
  include_directories(${CMAKE_SOURCE_DIR}/extlibs/headers/)

  add_definitions(-DGLEW_STATIC)

  # Do not compile glew (only for windows).
  list(REMOVE_ITEM source_files ${SRC}/glew/glew.c)

#------------
#   Mac
#------------
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(spacecraft_executable "spacecraft")
  list(APPEND CMAKE_LIBRARY_PATH ./extlibs/libs-osx)
  include_directories(${CMAKE_SOURCE_DIR}/extlibs/headers/)

  add_custom_target(osxfix
    COMMAND ${PROJECT_SOURCE_DIR}/osx-fix.sh bin/*
    DEPENDS
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  # Do not compile glew (only for windows).
  list(REMOVE_ITEM source_files ${SRC}/glew/glew.c)

#------------
#   Windows
#------------
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(spacecraft_executable "spacecraft.exe")
  list(APPEND CMAKE_LIBRARY_PATH ./extlibs/libs-win32)
  include_directories(${CMAKE_SOURCE_DIR}/extlibs/headers/)

  # ??? (smealum specifics ?)
  list(APPEND CMAKE_INCLUDE_PATH "C:/MinGW/include")
  list(APPEND CMAKE_INCLUDE_PATH "C:/CodeBlocks/MinGW/include")
  list(APPEND CMAKE_LIBRARY_PATH "C:/MinGW/lib")
  list(APPEND CMAKE_LIBRARY_PATH "C:/CodeBlocks/MinGW/lib")

  # Copy the dll in the bin directory.
  file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  file(GLOB_RECURSE dll_files extlibs/dll/*)
  foreach(file ${dll_files})
    get_filename_component(name ${file} NAME)
    configure_file(${file} ${EXECUTABLE_OUTPUT_PATH} COPYONLY)
  endforeach()
  
  # ???
  if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
    list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,-subsystem,windows")
  endif()

  add_definitions(-DGLEW_NO_GLU)
  add_definitions(-DGLEW_BUILD)
  add_definitions(-DGLEW_STATIC)
endif()

add_executable( ${spacecraft_executable} ${source_files})

if(${OS_OSX})
	add_dependencies(${spacecraft_executable} fixosx)
endif()

# OpenGL
if (NOT Web)
  find_package(OpenGL REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIR})
  target_link_libraries(${spacecraft_executable} ${OPENGL_LIBRARIES})
endif()

# glew
if (NOT Web)
  find_package(GLEW REQUIRED)
  include_directories(${GLEW_INCLUDE_PATH})
  target_link_libraries(${spacecraft_executable} ${GLEW_LIBRARY})
endif()

# glfw3
if (NOT Web) 
  find_package(glfw3 REQUIRED)
  include_directories(${GLFW_INCLUDE_DIRS})
  target_link_libraries(${spacecraft_executable} ${GLFW_LIBRARIES})
  message(STATUS arthur = ${GLFW_LIBRARIES})
endif()

# AntTweakBar
if (NOT Web)
  find_package(AntTweakBar REQUIRED)
  include_directories(${ANTTWEAKBAR_INCLUDE_DIRS})
  target_link_libraries(${spacecraft_executable} ${ANTTWEAKBAR_LIBRARY})
else()
  add_definitions(-DNTWBAR)
endif()

# libnoise
if (NOT Web)
  find_package(noise REQUIRED)
  include_directories(${NOISE_INCLUDE_DIRS})
  target_link_libraries(${spacecraft_executable} ${NOISE_LIBRARY})
endif()

# SFML
if (NOT Web)
  find_package(SFML 2 COMPONENTS system REQUIRED)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${spacecraft_executable} ${SFML_LIBRARIES})
endif()

# ??? macos specifics?
SET(CMAKE_FIND_FRAMEWORK NEVER)

if (${OS_OSX})
	add_custom_target(run
		COMMAND ${EXECUTABLE_OUTPUT_PATH}/${spacecraft_executable}
		DEPENDS ${spacecraft_executable} osxfix
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
else()
	add_custom_target(run
		COMMAND ${EXECUTABLE_OUTPUT_PATH}/${spacecraft_executable}
		DEPENDS ${spacecraft_executable}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
endif()

# Generate documentation -------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
