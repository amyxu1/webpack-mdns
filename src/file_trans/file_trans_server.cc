#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include <iostream>
#include "include/file_trans.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using file_trans::FileChunk;
using file_trans::Request;
using file_trans::WebpackServer;

static const int FILE_CHUNK_SIZE_BYTES = 1024;

class WebpackServerImpl final : public WebpackServer::Service
{

public: 
  Status SendFile(ServerContext* context, 
            const Request* request, 
            ServerWriter<FileChunk>* writer) override 
  {
    std::ifstream fileBuffer(request->filename(), std::ios::in|std::ios::binary);
    fileBuffer.seekg(0, fileBuffer.beg);
    char inputBuffer[FILE_CHUNK_SIZE_BYTES];
    while (fileBuffer.good())
    {
      FileChunk fileChunk;
      fileChunk.clear_chunk();  // ensure empty chunk
      fileBuffer.read(inputBuffer, FILE_CHUNK_SIZE_BYTES);
      fileChunk.set_chunk((void*)inputBuffer, fileBuffer.gcount());
      writer->Write(fileChunk);
    }

    return Status::OK;
  }
};

int main(int argc, char** argv)
{
  WebpackServerImpl webpack;
  ServerBuilder builder;
  std::string server_address = argv[1]/*"127.0.0.1:50051"*/;
  WebpackServerImpl service;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "server listening" << std::endl;
  server->Wait();	
  return 0;
}
