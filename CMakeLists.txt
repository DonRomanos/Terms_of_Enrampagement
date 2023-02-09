cmake_minimum_required(VERSION 3.12)

project(Terms_of_Enrampagement)

#*******************************************************************
# User options

option(BUILD_BENCHMARK "This will enable the performance tests using google benchmark" ON)
option(BUILD_TESTS "Will build the tests verifying your result" ON)

# This project is structurized into different main parts, which are as follows:

# Program: basically the main setting up the dependencies, this is the executable defined in this CMakelists.txt
# Core:    everything that is shared or does not belong to any other part and is used by many other parts, e.g. clock, network, etc.
# Input:   responsible for any kind of user information
# Graphics: this is the implementation of the graphics engine
# Simulation: this contains the game state and logic and advances the logic based on the inputs

# Every module has its files in a subdirectory.
# Glfw is used as a dependency for window management and maths

find_package(glfw3 REQUIRED)
find_package(range-v3 REQUIRED)

#*******************************************************************************
# Submodules
add_subdirectory(src/core)
add_subdirectory(src/input)
add_subdirectory(src/simulation)
#add_subdirectory(src/graphics)
add_subdirectory(src/benchmark)
add_subdirectory(src/program)

#***********************************************************************************
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    input::glfw_action_factory
    #simulation::logic
    #input::provider
    #...
)

target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -pedantic -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX /std:c++latest>
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

######################################################################
# Should be extracted each subdir should have its own tests!
if( BUILD_TESTS)
    find_package(GTest REQUIRED)

    # Actual tests will be defined within subdirectories!
    add_executable(${PROJECT_NAME}_Tests)
    target_sources(${PROJECT_NAME}_Tests PRIVATE 
        src/tests/tests.cpp)

    target_include_directories(${PROJECT_NAME}_Tests PRIVATE
        src/)

    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE
        GTest::gtest
    )

    include(GoogleTest)
    enable_testing()
    gtest_discover_tests(${PROJECT_NAME}_Tests)
endif()