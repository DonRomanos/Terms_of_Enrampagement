# Same pattern to reduce dependencies, one interface library, another one for the actual implementation.
add_library(key_to_command_translator INTERFACE)
add_library(input::key_to_command_translator ALIAS key_to_command_translator)

# Hopefully we stay with very few dependencies
target_sources(key_to_command_translator INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/entity_commands.hpp
    ${CMAKE_CURRENT_LIST_DIR}/input_translator.hpp
)

#***************************************************************
add_library(glfw_translator)
add_library(input::glfw_translator ALIAS glfw_translator)

target_sources(glfw_translator PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/glfw_input_translator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/input_command_table.cpp
)

target_include_directories(glfw_translator PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(glfw_translator PUBLIC
    input::key_to_command_translator
PRIVATE
    glfw::glfw
    graphics::engine
    range-v3::range-v3
)

target_compile_options(glfw_translator PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -pedantic -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /std:c++latest /permissive- /experimental:preprocessor> # most of it required for range-v3
)

target_compile_features(glfw_translator PRIVATE cxx_std_20)
set_property(TARGET glfw_translator PROPERTY CXX_STANDARD 20)

#***************************************************************
if( BUILD_TESTS)
    find_package(gtest REQUIRED)

    add_executable(glfw_translator_tests)
    target_sources(glfw_translator_tests PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/tests/input_tests.cpp
    )

    target_include_directories(glfw_translator_tests PRIVATE
        src/)

    target_link_libraries(glfw_translator_tests PRIVATE
        gtest::gtest
    )

    include(GoogleTest)
    enable_testing()
    gtest_discover_tests(glfw_translator_tests)
endif()