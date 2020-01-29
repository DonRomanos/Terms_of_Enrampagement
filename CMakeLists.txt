cmake_minimum_required(VERSION 3.12)

project(Terms_of_Enrampagement)

add_library(${PROJECT_NAME}_logic INTERFACE)
add_library(logic::${PROJECT_NAME} ALIAS ${PROJECT_NAME}_logic)

target_include_directories(${PROJECT_NAME}_logic INTERFACE src/)
target_sources(${PROJECT_NAME}_logic INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dispatcher.hpp)
# Normally I would make this private, but since this is my own and I am lazy I can do this :)
target_compile_features(${PROJECT_NAME}_logic INTERFACE cxx_std_20)
target_compile_options(${PROJECT_NAME}_logic INTERFACE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

find_package(glfw REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw_dispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan_renderer.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE
    logic::${PROJECT_NAME}
    glfw::glfw
    Vulkan::Vulkan)
target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)
######################################################################
option(BUILD_TESTS "Will build the tests verifying your result" ON)
if( BUILD_TESTS)
    find_package(gtest REQUIRED)
    add_executable(${PROJECT_NAME}_Tests)
    target_sources(${PROJECT_NAME}_Tests PRIVATE 
        src/tests/tests.cpp)
    target_include_directories(${PROJECT_NAME}_Tests PRIVATE
        src/)
    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE
        logic::${PROJECT_NAME}
        gtest::gtest)

    include(GoogleTest)
    enable_testing()
    gtest_discover_tests(${PROJECT_NAME}_Tests)
endif()

######################################################################
option(BUILD_BENCHMARK "This will enable the performance tests using google benchmark" ON)
if( BUILD_BENCHMARK)
    find_package(benchmark REQUIRED)
    # Requires PThreads on Linux
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    add_executable(${PROJECT_NAME}_Benchmark)
    target_sources(${PROJECT_NAME}_Benchmark PRIVATE 
        src/benchmark/benchmark.cpp)
    target_include_directories(${PROJECT_NAME}_Benchmark PRIVATE
        src
        src/benchmark)
    target_link_libraries(${PROJECT_NAME}_Benchmark PRIVATE
        logic::${PROJECT_NAME}
        benchmark::benchmark
        Threads::Threads)

    add_custom_target(NAME ${PROJECT_NAME}_Benchmark
        COMMAND ${PROJECT_NAME}_Benchmark
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
