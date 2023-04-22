cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set the project name
project(deepdrill)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Set the optimization level (if not specified on the command line)
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

# Find third-party packages
find_package(PkgConfig)
pkg_check_modules(GMP gmpxx)
pkg_check_modules(SFML sfml-system sfml-window sfml-graphics)

# Specify the executable
add_executable(deepdrill ddrill/DeepDrill.cpp)
add_executable(deepflight dflight/DeepFlight.cpp)

# Specify compile options
target_compile_options(deepdrill PUBLIC -Wall -Werror)
target_compile_options(deepdrill PUBLIC -Wno-unused-parameter)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(deepdrill PUBLIC -Wno-restrict)
else()
    # target_compile_options(deepdrill PUBLIC -Wno-gnu-anonymous-struct)
    # target_compile_options(deepdrill PUBLIC -Wno-nested-anon-types)
endif()

# Specify link options
target_link_options(deepdrill PUBLIC ${GMP_LDFLAGS})
target_link_options(deepdrill PUBLIC ${SFML_LDFLAGS})
target_link_options(deepflight PUBLIC ${GMP_LDFLAGS})
target_link_options(deepflight PUBLIC ${SFML_LDFLAGS})

# Add include paths
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepdrill PUBLIC ${GMP_INCLUDE_DIRS})
target_include_directories(deepdrill PUBLIC ${SFML_INCLUDE_DIRS})
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC ${GMP_INCLUDE_DIRS})
target_include_directories(deepflight PUBLIC ${SFML_INCLUDE_DIRS})

# Add sub directories
add_subdirectory(util)
add_subdirectory(math)
add_subdirectory(shared)
add_subdirectory(ddrill)
add_subdirectory(dflight)
