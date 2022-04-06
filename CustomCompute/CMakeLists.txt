cmake_minimum_required(VERSION 3.18)

project(CustomCompute)

find_package(Vulkan REQUIRED)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)


# add_custom_command(
#     COMMENT "Buiding HLSL Shaders"    
#     DEPENDS "shader.hlsl"
#     OUTPUT "${CMAKE_BINARY_DIR}/compute.spv"
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     COMMAND glslc -fshader-stage=compute "shader.hlsl" -o "${CMAKE_BINARY_DIR}/bin/compute.spv"
# )
# add_custom_target(ComputeShaderHLSL DEPENDS "${CMAKE_BINARY_DIR}/compute.spv")

add_custom_command(
    COMMENT "Buiding GLSL Shaders"    
    DEPENDS "shader.comp"
    OUTPUT "${CMAKE_BINARY_DIR}/compute.spv"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND glslc "shader.comp" -o "${CMAKE_BINARY_DIR}/bin/compute.spv"
)
add_custom_target(ComputeShaderGLSL DEPENDS "${CMAKE_BINARY_DIR}/compute.spv")

add_executable(CustomCompute "main.cpp")
target_link_libraries(CustomCompute PRIVATE Vulkan::Vulkan)
add_dependencies(CustomCompute ComputeShaderGLSL)
# add_dependencies(CustomCompute ComputeShaderHLSL)