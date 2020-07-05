cmake_minimum_required ( VERSION 3.0 )

project(Client LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(client client.c)