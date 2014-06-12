cmake_minimum_required(VERSION 2.6)

# define a macro that helps defining an option
macro(set_option var default type docstring)
  if(NOT DEFINED ${var})
    set(${var} ${default})
  endif()
  set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

set_option(CMAKE_BUILD_TYPE RELEASE STRING "DEBUG: -g, RELEASE: -O3")

if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
  add_definitions(-DNDEBUG)
  #add_definitions(-DNTWBAR)
endif()

project(spacecraft)

# bin dir
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Configure OS versions with some variables
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/extlibs/headers)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(OS_WINDOWS 1)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} C:/MinGW/lib C:/CodeBlocks/MinGW/lib ${PROJECT_SOURCE_DIR}/extlibs/libs-win32)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} C:/MinGW/include C:/CodeBlocks/MinGW/include )
  add_definitions(-DGLEW_NO_GLU -DGLEW_BUILD -DGLEW_STATIC)
  # create the bin dir and copy the dlls
  file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  file(GLOB_RECURSE dll_files extlibs/dll/*)
  foreach(file ${dll_files})
    get_filename_component(name ${file} NAME)
    configure_file(${file} ${EXECUTABLE_OUTPUT_PATH} COPYONLY)
  endforeach()
  if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
  endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(OS_LINUX 1)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/extlibs/libs-linux)
  add_definitions(-DGLEW_STATIC)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OS_OSX 1)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/extlibs/libs-osx)
else()
  message("FATAL: Unsupported OS")
  return()
endif()

# set some options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-reorder -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -O3")
# -g is there by default
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message("WARNING: ${PROJECT_NAME} requires g++ 4.7 or greater in order to use c++11. Instead c++0x will be used.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX} -std=c++0x")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

set(SRC ${PROJECT_SOURCE_DIR}/src)
include_directories(${SRC})

# modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# find files
file(GLOB_RECURSE source_files ${SRC}/*)

# Do not compile glew on linux and osx
#if (NOT OS_WINDOWS)
  list(REMOVE_ITEM source_files ${SRC}/glew/glew.c)
  find_package(GLEW REQUIRED)
#endif()

# Testing executable
set(EXEC spacecraft)
add_executable(
  ${EXEC}
  ${source_files}
)

# Finding libs
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(AntTweakBar REQUIRED)
find_package(noise REQUIRED)
SET(CMAKE_FIND_FRAMEWORK NEVER)
find_package(SFML 2 COMPONENTS system REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_PATH} ${GLFW_INCLUDE_DIR} ${ANTTWEAKBAR_INCLUDE_DIR} ${NOISE_INCLUDE_DIR} ${SFML_INCLUDE_DIR})

# libs
target_link_libraries(${EXEC} ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${ANTTWEAKBAR_LIBRARY} ${NOISE_LIBRARY} ${SFML_LIBRARIES})

add_custom_target(run
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/${EXEC}
  DEPENDS ${EXEC}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(valgrind
  COMMAND valgrind --leak-check=full --tool=memcheck
  ${EXECUTABLE_OUTPUT_PATH}/${EXEC}
  DEPENDS ${EXEC}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

# tests
enable_testing()
SET(OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${EXEC}.dir/src)
#message("OBJ: ${OBJ_FILES}")
file(GLOB_RECURSE all_obj ${OBJ_FILES}/*.o)
list(REMOVE_ITEM all_obj ${OBJ_FILES}/main.cpp.o)
#STRING(REPLACE ; " " all_obj "${all_obj}")
file(GLOB_RECURSE all_tests ${PROJECT_SOURCE_DIR}/tests/*.cpp)
#message("tests: ${all_tests}")

foreach(t ${all_tests})
	GET_FILENAME_COMPONENT(f_base ${t} NAME_WE)
	add_executable(${f_base} ${t} ${all_obj})
	add_dependencies(${f_base} ${EXEC})
	target_link_libraries(${f_base} ${GLEW_LIBRARY} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${ANTTWEAKBAR_LIBRARY} ${NOISE_LIBRARY} ${SFML_LIBRARIES})
	add_test(${f_base}
		${EXECUTABLE_OUTPUT_PATH}/${f_base}
		${PROJECT_SOURCE_DIR})
	#add_test(NAME ${EXEC}
	#WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	#COMMAND ${EXECUTABLE_OUTPUT_PATH}/${EXEC})
endforeach(t)

# Doc

find_package(Doxygen)
if(DOXYGEN_FOUND)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
