cmake_minimum_required ( VERSION 3.0 )

project( Server LANGUAGES C )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
include_directories(../daemon)
add_executable(server server.c)
add_executable(init initialize.c)
target_link_libraries(server Daemon)

