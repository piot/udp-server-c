cmake_minimum_required(VERSION 3.17)
project(udp-server-example C)

set(CMAKE_C_STANDARD 99)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(isDebug TRUE)
else()
    message("no debug")
    set(isDebug FALSE)
endif()

if (WIN32)
set(OS_WINDOWS 1)
endif()




if (isDebug)
        message("Debug build detected")
        add_compile_definitions(CONFIGURATION_DEBUG TYRAN_CONFIGURATION_DEBUG)
        set(CONFIGURATION_DEBUG 1)
endif()


set(deps ../deps/)

include_directories(${deps}piot/clog/src/include)
include_directories(${deps}piot/tiny-libc/src/include)
include_directories(include)

file(GLOB_RECURSE deps_src FOLLOW_SYMLINKS
        "${deps}*/src/lib/*.c"
        )

file(GLOB_RECURSE lib_src FOLLOW_SYMLINKS
        "lib/*.c"
        )

add_executable(udp-server-example
    ${deps_src}
    ${lib_src}
    examples/main.c
)

if (OS_WINDOWS)
target_compile_definitions(udp-server-example PUBLIC TORNADO_OS_WINDOWS)
endif()

if(MSVC)
  target_compile_options(udp-server-example PRIVATE /W4 /WX)
else()
  target_compile_definitions(udp-server-example PRIVATE CRT_SECURE_NO_WARNINGS)
  target_compile_options(udp-server-example PRIVATE -Wall -Wextra -Wpedantic -Wdeprecated-declarations)
endif()

if (OS_WINDOWS)
target_link_libraries(udp-server-example ws2_32)
else()
target_link_libraries(udp-server-example m)
endif()