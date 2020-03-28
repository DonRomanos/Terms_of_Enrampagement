# For now we have only headers
add_library(core INTERFACE)
add_library(core::core ALIAS core)

# Hopefully we stay with very few dependencies
target_sources(core INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/actions.hpp
    ${CMAKE_CURRENT_LIST_DIR}/gamestate.hpp
    )

target_include_directories(core INTERFACE ${CMAKE_CURRENT_LIST_DIR})