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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

# Find third-party packages
find_package(PkgConfig)
pkg_check_modules(GMP gmpxx)
pkg_check_modules(SFML sfml-system sfml-window sfml-graphics)

# Specify the executable
add_executable(deepdrill ddrill/DeepDrill.cpp)
add_executable(deepflight dflight/DeepFlight.cpp)

# Add include paths
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepdrill PUBLIC ${GMP_INCLUDE_DIRS})
target_include_directories(deepdrill PUBLIC ${SFML_INCLUDE_DIRS})
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC ${GMP_INCLUDE_DIRS})
target_include_directories(deepflight PUBLIC ${SFML_INCLUDE_DIRS})

# Specify compile options
target_compile_options(deepdrill PUBLIC -Wall -Werror)
target_compile_options(deepdrill PUBLIC -Wno-unused-parameter)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(deepdrill PUBLIC -Wno-restrict)
else()
endif()

# Specify link options
target_link_libraries(deepdrill PUBLIC ${GMP_LDFLAGS} ${SFML_LDFLAGS})
target_link_libraries(deepflight PUBLIC ${GMP_LDFLAGS} ${SFML_LDFLAGS})

# Add sub directories
add_subdirectory(util)
add_subdirectory(math)
add_subdirectory(shared)
add_subdirectory(ddrill)
add_subdirectory(dflight)

# Copy asset directories
set(directories tutorial profiles palettes shaders)
foreach(dir ${directories})
add_custom_command(
        TARGET deepdrill POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../${dir}
                ${CMAKE_CURRENT_BINARY_DIR}/${dir})
endforeach()
