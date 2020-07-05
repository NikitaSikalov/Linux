cmake_minimum_required (VERSION 3.0)

project(lib)

# http://www.mariobadr.com/creating-a-header-only-library-with-cmake.html
add_library(lib INTERFACE)
target_include_directories(lib INTERFACE ./)