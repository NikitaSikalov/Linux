cmake_minimum_required ( VERSION 3.0 )

project( client-server LANGUAGES C )

add_subdirectory(daemon)
add_subdirectory(fs)
add_subdirectory(client)