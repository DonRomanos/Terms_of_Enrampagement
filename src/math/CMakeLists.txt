add_library(math)
add_library(math::math ALIAS math)

target_include_directories(math
    PUBLIC
        include
)

target_sources(math
    PRIVATE
        include/math/vector.hpp
        vector.cpp
)

#***************************************************************
if(BUILD_TESTING)
    add_executable(math_tests)
    target_sources(math_tests PRIVATE 
       test/vector_tests.cpp
    )

    target_link_libraries(math_tests PRIVATE
        math::math
        GTest::gtest_main
    )

    gtest_discover_tests(math_tests)
endif()
