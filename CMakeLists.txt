cmake_minimum_required(VERSION 3.16)

project(
    CloudVSE
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic"
)

add_subdirectory(call-network)
add_subdirectory(billing)
add_subdirectory(call-control)
add_subdirectory(simulator)
