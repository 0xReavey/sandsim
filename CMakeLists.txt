cmake_minimum_required(VERSION 3.16)
project(sandsim)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Werror -O3")

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add SFML library
find_package(SFML 2.5 COMPONENTS graphics window system)

# Add source files
file(GLOB_RECURSE SRC_FILES src/*.cpp src/elements/*.cpp src/world/*.cpp)

# Add include directories
include_directories(src/elements src/world)

# Create executable
add_executable(sandsim ${SRC_FILES})

# Link SFML library
target_link_libraries(sandsim sfml-graphics sfml-window sfml-system)
