cmake_minimum_required ( VERSION 3.0 )

project( file-system LANGUAGES C )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/sikalov/mipt/Linux")
#set(FILE_STORE "bin/fs_store")

add_subdirectory(daemon)
include_directories(daemon)
#add_library(daemon daemon/demonize.c)
#add_executable(fs fs/main.c)
#add_executable(init fs/initialize.c fs/base_structures.h fs/fs_store_functions.h fs/fs_base.h daemon/demonize.h)
add_executable(Main main.c)
target_link_libraries(Main Daemon)