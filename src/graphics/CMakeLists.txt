# To break depdencies we have two targets, one is the interface for the engine the other one the actual implementation
add_library(engine INTERFACE)
add_library(graphics::engine ALIAS engine)

# This will depend on the simulation state object / commands
target_sources(engine INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/graphics_engine.hpp
)

#***************************************************************
add_library(vulkan_engine)
add_library(graphics::vulkan_engine ALIAS vulkan_engine)

target_sources(vulkan_engine PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/vulkan_renderer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vulkan_setup_helper.cpp
)

target_include_directories(vulkan_engine PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(vulkan_engine PUBLIC
#TODO remove this from public dependencies!
    glfw::glfw
    Vulkan::Vulkan
PRIVATE
    graphics::engine
    range-v3::range-v3
)

target_compile_options(vulkan_engine PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -pedantic -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /std:c++latest /permissive- /experimental:preprocessor> # most of it required for range-v3
)

target_compile_features(vulkan_engine PRIVATE cxx_std_20)
set_property(TARGET vulkan_engine PROPERTY CXX_STANDARD 20)

#**********************************************************
add_subdirectory(shaders)