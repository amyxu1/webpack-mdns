project(WebpackServer)
add_library(${PROJECT_NAME} file_trans_server.cc file_trans_client.cc)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GRPCPP REQUIRED grpc++)
find_package(protobuf REQUIRED)
find_package(grpc REQUIRED)
target_link_libraries(WebpackServer PUBLIC file_trans.grpc)
