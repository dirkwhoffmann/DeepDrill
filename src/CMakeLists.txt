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

# Look for GMP library
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(GMP REQUIRED)

# Specify the executable
add_executable(deepdrill ddrill/DeepDrill.cpp)
add_executable(deepflight dflight/DeepFlight.cpp)

# Specify compile options
target_compile_options(deepdrill PUBLIC -Wall -Werror)
target_compile_options(deepdrill PUBLIC -Wno-unused-parameter)
target_compile_options(deepdrill PUBLIC -Wno-gnu-anonymous-struct)
target_compile_options(deepdrill PUBLIC -Wno-nested-anon-types)

# Specify link options
target_link_options(deepdrill PUBLIC -L/opt/homebrew/Cellar/gmp/6.2.1_1/lib)
target_link_options(deepdrill PUBLIC -lgmpxx -lgmp)
target_link_options(deepflight PUBLIC -L/opt/homebrew/Cellar/gmp/6.2.1_1/lib)
target_link_options(deepflight PUBLIC -L/opt/homebrew/Cellar/sfml/2.5.1_2/lib)
target_link_options(deepflight PUBLIC -lgmpxx -lgmp)
target_link_options(deepflight PUBLIC -lsfml-system -lsfml-window -lsfml-graphics)

# Add include paths
target_include_directories(deepdrill PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepdrill PUBLIC /opt/homebrew/Cellar/gmp/6.2.1_1/include)
target_include_directories(deepflight PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(deepflight PUBLIC /opt/homebrew/Cellar/gmp/6.2.1_1/include)
target_include_directories(deepflight PUBLIC /opt/homebrew/Cellar/sfml/2.5.1_2/include)

# Add sub directories
add_subdirectory(util)
add_subdirectory(math)
add_subdirectory(ddrill)
add_subdirectory(dflight)
