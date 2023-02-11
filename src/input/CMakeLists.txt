add_library(input_interface INTERFACE)
add_library(input::interface ALIAS input_interface)

target_include_directories(input_interface
    INTERFACE
        include
)

target_sources(input_interface
    INTERFACE
        include/input/provider.hpp
)

#***************************************************************
add_library(input_glfw)
add_library(input::glfw ALIAS input_glfw)

target_sources(input_glfw PRIVATE
    glfw_input_provider.hpp
    glfw_input_provider.cpp
)

target_link_libraries(input_glfw PUBLIC
    input::interface
    core::core
PRIVATE
    glfw
)

#***************************************************************
if(BUILD_TESTING)
    add_executable(glfw_input_tests)
    target_sources(glfw_input_tests PRIVATE 
       test/glfw_input_tests.cpp
    )

    target_link_libraries(glfw_input_tests PRIVATE
        input::glfw
        GTest::gtest_main
    )

    gtest_discover_tests(glfw_input_tests)
endif()
