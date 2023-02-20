include_guard(GLOBAL)

# Cache is needed so that vscode cmake-tools extension knows which debugger to use
set(CMAKE_CXX_COMPILER clang++-15 CACHE STRING "CXX Compiler" FORCE)
set(CMAKE_C_COMPILER clang-15 CACHE STRING "C Compiler" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
