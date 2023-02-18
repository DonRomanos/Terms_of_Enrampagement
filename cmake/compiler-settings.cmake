set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(msvc)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wconversion
        -Wsign-conversion 
        -pedantic-errors)
endif()
