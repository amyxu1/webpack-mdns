# Minimum CMake required 
cmake_minimum_required(VERSION 3.14.4)
#project(controller)

set(EXTRA_LIBS ${EXTRA_LIBS} mdns)
#set(EXTRA_LIBS ${EXTRA_LIBS} grpc)
set(EXTRA_LIBS ${EXTRA_LIBS} NetworkController)
set(EXTRA_LIBS ${EXTRA_LIBS} WebpackServer)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::container)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::container_memory)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::core_headers)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::hash)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::hash_function_defaults)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::hashtablez_sampler)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::memory)
set(EXTRA_LIBS ${EXTRA_LIBS} absl::raw_hash_map)


set(GCC_COVERAGE_LINK_FLAGS "-lgrpc++_reflection")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Project
add_subdirectory(network_controller)
add_subdirectory(file_trans)
add_executable(mdns-controller binary/main.cpp)
target_link_libraries(mdns-controller ${EXTRA_LIBS})
#target_link_libraries(mdns-controller absl::container absl::container_memory absl::core_headers absl::hash absl::hash_function_defaults absl::hashtablez_sampler absl::memory absl::raw_hash_map)
