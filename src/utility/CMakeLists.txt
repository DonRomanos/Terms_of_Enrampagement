add_library(utility STATIC)
add_library(utility::utility ALIAS utility)

target_sources(utility 
    PRIVATE
        include/utility/order_independent_vector.hpp
        order_independent_vector.cpp
)

target_include_directories(utility PUBLIC include)

#***************************************************************
if(BUILD_TESTING)
    add_executable(utility_tests)
    target_sources(utility_tests PRIVATE 
       test/order_independent_vector_tests.cpp
    )

    target_link_libraries(utility_tests PRIVATE
        utility::utility
        GTest::gtest_main
    )

    gtest_discover_tests(utility_tests)
endif()
