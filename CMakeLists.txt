cmake_minimum_required(VERSION 3.10)
project(Raytracing C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add executable
add_executable(Raytracing main.c)

# Link SDL2 and math library
target_link_libraries(Raytracing ${SDL2_LIBRARIES} m)
