cmake_minimum_required(VERSION 3.5)

project(mdns-controller)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
add_subdirectory(src)
add_subdirectory(thirdparty)

