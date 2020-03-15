if( BUILD_BENCHMARK)
    find_package(benchmark REQUIRED)
    # Requires PThreads on Linux
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    add_executable(benchmark)
    add_executable(${PROJECT_NAME}::benchmark ALIAS benchmark)

    target_sources(benchmark PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/benchmark.cpp
    )

    target_include_directories(benchmark PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
    )

    target_link_libraries(benchmark PRIVATE
        benchmark::benchmark
        Threads::Threads)

    add_custom_target(NAME run_benchmark
        COMMAND run_benchmark
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()
