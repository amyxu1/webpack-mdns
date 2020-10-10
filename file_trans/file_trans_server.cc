#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include <iostream>
#include "file_trans.grpc.pg.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using file_trans::FileChunk;
using file_trans::Request;

static const FILE_CHUNK_SIZE_BYTES = 1024;

class WebpackServerImpl final : public WebpackServer::Service
{

public: 
  Status SendFile(ServerContext* context, 
            const Request* request, 
            ServerWriter<Message>* writer) override 
  {
    ifstream fileBuffer(request.filename(), std::ios::in|std::ios::binary);
    fileBuffer.seekg(0, fileBuffer.beg);
    char inputBuffer[FILE_CHUNK_SIZE_BYTES];

    while (fileBuffer.good())
    {
      FileChunk fileChunk;
      fileChunk.clear_chunk();  // ensure empty chunk
      fileBuffer.read(inputBuffer, FILE_CHUNK_SIZE_BYTES);
      fileChunk.setChunk((void*)inputBuffer, fileBuffer.gcount());
      writer->Write(fileChunk);
    }

    return Status::OK;
  }
};
