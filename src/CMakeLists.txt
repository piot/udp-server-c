cmake_minimum_required(VERSION 3.16.3)
project(udp-server C)

add_subdirectory("deps/piot/clog/src/lib")
add_subdirectory("deps/piot/tiny-libc/src/lib")

add_subdirectory("lib")