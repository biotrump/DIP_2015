# DIP Homework Assignment #1
# March  10, 2016
# Name: Thomas Tsai
# ID #: D04922009
# email: d04922009@ntu.edu.tw, thomas@life100.cc
# compiled on ubuntu 14.04 with gcc-4.9, opencv 2.4.x
cmake_minimum_required(VERSION 2.8.10)
project(DIP_HW C CXX)

message("CMAKE_SOURCE_DIR : ${CMAKE_SOURCE_DIR}")
message("PROJECT_NAME : ${PROJECT_NAME}")
message("DIP_HW_SOURCE_DIR : ${DIP_HW_SOURCE_DIR}")
message("DIP_HW_BINARY_DIR : ${DIP_HW_BINARY_DIR}")

#equivalent to  "cmake -DCMAKE_BUILD_TYPE=Release"
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
message("***CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
# --------------------------------------------------
# First, define all the compilation options.
# We default to debugging mode for developers.
option(DEBUG "Compile with debugging information" OFF)

if(DEBUG)
set(CMAKE_BUILD_TYPE "Debug")
endif(DEBUG)

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -O0 -fPIC -D_POSIX_C_SOURCE=200809L")
else("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  add_definitions(-DARMA_NO_DEBUG)
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O0 -g -fPIC -D_POSIX_C_SOURCE=200809L")
endif("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
# --------------------------------------------------
# Organize output files.  On Windows this also keeps .dll files next
# to the .exe files that need them, making tests easy to run.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIP_HW_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIP_HW_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIP_HW_BINARY_DIR}/lib)
# --------------------------------------------------
#QT5
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets)
#  find_package(Qt5Declarative)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
#  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# --------------------------------------------------
#opencv should be built first!
#set(OpenCV_DIR $ENV{OPENCV_OUT})
message("++++OpenCV_DIR:${OpenCV_DIR}")
FIND_PACKAGE( OpenCV REQUIRED)
if(OpenCV_FOUND)
	message("found")
	message( "OpenCV_INCLUDE_DIRS:${OpenCV_INCLUDE_DIRS}" )
	message( "OpenCV_INCLUDES:${OpenCV_INCLUDES}")
	message( "OpenCV_LIBRARIES:${OpenCV_LIBRARIES}")
	message( "OpenCV_LIBS:${OpenCV_LIBS}")
	message( "OpenCV_DEFINITIONS:${OpenCV_DEFINITIONS}")
else()
	message(FATAL_ERROR "openCV not foud")
endif()

message("*** OpenCV_LIBS:${OpenCV_LIBS}")
message("*** OpenCV_LIBRARIES:${OpenCV_LIBRARIES}")
# --------------------------------------------------
#openMP to avoid link error : omp_xxx error!
# undefined reference to `GOMP_loop_dynamic_start
if( NOT OPENMP_FOUND)
	find_package(OpenMP)
	if(OPENMP_FOUND)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	endif()
endif( NOT OPENMP_FOUND)
# --------------------------------------------------
# Make a target to generate the documentation.  If Doxygen isn't installed, then
# I guess this option will just be unavailable.
if (NOT DOXYGEN_FOUND)
	find_package(Doxygen)

	if(ANDROID_NDK)
	find_host_package(Doxygen)
	endif(ANDROID_NDK)

	if (DOXYGEN_FOUND)
	# Preprocess the Doxyfile.  This is done before 'make doc'.
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/Doxyfile
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -D DESTDIR="${CMAKE_BINARY_DIR}" -P
			"${CMAKE_CURRENT_SOURCE_DIR}/CMake/GenerateDoxyfile.cmake"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Creating Doxyfile to generate Doxygen documentation"
	)

	# Generate documentation.
	add_custom_target(doc
		COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/Doxyfile"
		DEPENDS "${CMAKE_BINARY_DIR}/Doxyfile"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		COMMENT "Generating API documentation with Doxygen"
	)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html
		DESTINATION share/doc/mlpack
		COMPONENT doc
		OPTIONAL
	)
	endif (DOXYGEN_FOUND)
endif ()
# --------------------------------------------------
# Executables
# --------------------------------------------------
# To show a rawfile
# --------------------------------------------------
add_executable(raw_show
	raw_show.cpp
)
qt5_use_modules(raw_show Widgets  OpenGL Test Concurrent)
target_link_libraries(raw_show
	${OpenCV_LIBS}
)
# --------------------------------------------------
# To flip a rawfile
# --------------------------------------------------
add_executable(flip
	flip.cpp
)
qt5_use_modules(flip Widgets  OpenGL Test Concurrent)
target_link_libraries(flip
	${OpenCV_LIBS}
)
# --------------------------------------------------
# PROBLEM 1: IMAGE ENHANCEMENT
# --------------------------------------------------
add_executable(p1
	p1.cpp
)
qt5_use_modules(p1 Widgets  OpenGL Test Concurrent)
target_link_libraries(p1
	${OpenCV_LIBS}
)
# --------------------------------------------------
# PROBLEM 2: NOISE REMOVAL
# --------------------------------------------------
add_executable(p2
	p2.cpp
)
qt5_use_modules(p2 Widgets  OpenGL Test Concurrent)
target_link_libraries(p2
	${OpenCV_LIBS}
)