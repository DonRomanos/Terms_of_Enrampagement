find_program(Vulkan_glslc NAME glslc HINTS $ENV{VULKAN_SDK}/bin $ENV{VULKAN_SDK}/bin32)
message(STATUS "Vulkan glslc:  ${Vulkan_glslc}")

add_custom_target(Compile_VertexShader ALL "${Vulkan_glslc}" "${CMAKE_CURRENT_LIST_DIR}/shader.vert" -o "${CMAKE_BINARY_DIR}/vert.spv")
add_custom_target(Compile_FragmentShader ALL "${Vulkan_glslc}" "${CMAKE_CURRENT_LIST_DIR}/shader.frag" -o "${CMAKE_BINARY_DIR}/frag.spv")