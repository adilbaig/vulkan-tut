cmake_minimum_required(VERSION 3.18)

# set the project name
project(HelloTriangle)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# add the executable
add_executable(HelloTriangle "main.cpp")

# set(CMAKE_EXE_LINKER_FLAGS "-lglfw -lvulkan")
target_link_libraries(HelloTriangle glfw vulkan)