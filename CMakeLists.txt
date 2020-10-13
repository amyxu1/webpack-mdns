cmake_minimum_required(VERSION 3.5)

project(mdns-controller)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)

# Process Abseil's CMake build system
add_subdirectory(abseil-cpp)
# Process mdns system
add_subdirectory(mdns)

add_executable(mdns-controller main.cpp NetworkController.hpp NetworkController.cpp)

# Declare dependency on the absl::strings library
find_package(Threads)
target_link_libraries(mdns-controller absl::algorithm_container absl::container absl::container_memory absl::core_headers absl::hash absl::hash_function_defaults absl::hashtablez_sampler absl::memory absl::raw_hash_map absl::raw_hash_set absl::strings Threads::Threads mdns)
target_include_directories(mdns-controller PUBLIC mdns abseil-cpp)
