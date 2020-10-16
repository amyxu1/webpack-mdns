project(NetworkController)

add_library(${PROJECT_NAME} src/NetworkController.cpp)


target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

find_package(Threads)
target_link_libraries(NetworkController 
	absl::algorithm_container 
	absl::container 
	absl::container_memory 
	absl::core_headers 
	absl::hash 
	absl::hash_function_defaults 
	absl::hashtablez_sampler 
	absl::memory 
	absl::raw_hash_map 
	absl::raw_hash_set 
	absl::strings 
	Threads::Threads 
	mdns)
