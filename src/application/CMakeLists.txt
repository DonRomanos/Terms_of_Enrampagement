add_library(application)
add_library(application::application ALIAS application)

target_include_directories(application
    PUBLIC
        include
)

target_sources(application
    PRIVATE
        include/application/application.hpp
        application.cpp
)

target_link_libraries(application 
    PRIVATE
        input::interface 
)
