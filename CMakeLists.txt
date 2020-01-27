cmake_minimum_required(VERSION 3.12)

project(Challenge)

add_library(${PROJECT_NAME} INTERFACE)
add_library(Challenge::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE src/)
target_sources(${PROJECT_NAME} INTERFACE
    src/challenge.hpp)
# Normally I would make this private, but since this is my own and I am lazy I can do this :)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_compile_options(${PROJECT_NAME} INTERFACE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

######################################################################
option(BUILD_REFERENCE "Will build a reference implementation as a library, 
                        which will also be tested and benchmarked. Cool for comparison." ON)
if(BUILD_REFERENCE)
    add_library(${PROJECT_NAME}_Reference INTERFACE)
    add_library(onions::${PROJECT_NAME}_Reference ALIAS ${PROJECT_NAME}_Reference)
    target_include_directories(${PROJECT_NAME}_Reference INTERFACE src/)
    target_sources(${PROJECT_NAME}_Reference INTERFACE
        src/reference.hpp)
    target_compile_definitions(${PROJECT_NAME}_Reference INTERFACE BUILD_REFERENCE) # We will use this for an ifdef
    target_compile_features(${PROJECT_NAME}_Reference INTERFACE cxx_std_20)
    target_compile_options(${PROJECT_NAME}_Reference INTERFACE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)
endif()

######################################################################
option(BUILD_TESTS "Will build the tests verifying your result" ON)
if( BUILD_TESTS)
    find_package(gtest REQUIRED)
    add_executable(${PROJECT_NAME}_Tests)
    target_sources(${PROJECT_NAME}_Tests PRIVATE 
        src/tests.cpp)
    target_include_directories(${PROJECT_NAME}_Tests PRIVATE
        src/)
    target_link_libraries(${PROJECT_NAME}_Tests PRIVATE
        Challenge::${PROJECT_NAME}
        gtest::gtest
        $<$<TARGET_EXISTS:${PROJECT_NAME}_Reference>:${PROJECT_NAME}_Reference>)

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
        src/benchmark.cpp)
    target_include_directories(${PROJECT_NAME}_Benchmark PRIVATE
        src/benchmark)
    target_link_libraries(${PROJECT_NAME}_Benchmark PRIVATE
        Challenge::${PROJECT_NAME}
        benchmark::benchmark
        Threads::Threads
        $<$<TARGET_EXISTS:${PROJECT_NAME}_Reference>:${PROJECT_NAME}_Reference>)

    add_custom_target(NAME ${PROJECT_NAME}_Benchmark
        COMMAND ${PROJECT_NAME}_Benchmark
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
