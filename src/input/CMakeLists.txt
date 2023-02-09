# Same pattern to reduce dependencies, one interface library, another one for the actual implementation.
add_library(action_factory INTERFACE)
add_library(input::action_factory ALIAS action_factory)

# Hopefully we stay with very few dependencies
target_sources(action_factory INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/input_provider.hpp
    ${CMAKE_CURRENT_LIST_DIR}/action_table.hpp
)

#***************************************************************
add_library(glfw_action_factory)
add_library(input::glfw_action_factory ALIAS glfw_action_factory)

target_sources(glfw_action_factory PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/glfw_input_provider.cpp
    ${CMAKE_CURRENT_LIST_DIR}/action_table.cpp
    ${CMAKE_CURRENT_LIST_DIR}/action_factory.cpp
)

target_include_directories(glfw_action_factory PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(glfw_action_factory PUBLIC
    input::action_factory
    core::core
PRIVATE
    glfw
)

target_compile_options(glfw_action_factory PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -pedantic -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /std:c++latest>
)

target_compile_features(glfw_action_factory PRIVATE cxx_std_20)
set_property(TARGET glfw_action_factory PROPERTY CXX_STANDARD 20)

#***************************************************************
if( BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_executable(glfw_action_factory_tests)
    target_sources(glfw_action_factory_tests PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/tests/action_factory_tests.cpp
    )

    target_include_directories(glfw_action_factory_tests PRIVATE
        src/)

    target_link_libraries(glfw_action_factory_tests PRIVATE
        input::glfw_action_factory
        GTest::gtest
    )

    target_compile_features(glfw_action_factory_tests PRIVATE cxx_std_20)
    set_property(TARGET glfw_action_factory_tests PROPERTY CXX_STANDARD 20)

    include(GoogleTest)
    enable_testing()
    gtest_discover_tests(glfw_action_factory_tests)
endif()