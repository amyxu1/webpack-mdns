#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include "file_trans.grpc.pb.h"
#include "file_trans.ph.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using file_trans::FileChunk;
using file_trans::Request;

class WebpackServerClient 
{

public:
  WebpackServerClient(std::shared_ptr<Channel> channel)
    : stub_(WebpackServer::NewStub(channel)) {}

  void SendFile(std::string filename)
  {
    ClientContext context;
    Request request;
    FileChunk fileChunk;

    request.set_filename(filename);
    std::unique_ptr<ClientReader<FileChunk>> reader(stub_->SendFile(&context, request));
    std::ofstream ofs;
    ofs.open(filename, std::ios::out|std::ios::binary);

    while (reader->Read(&fileChunk))
    {
      const std::string& chunk = fileChunk.chunk();
      ofs.write(chunk.c_str(), chunk.length());
    }
    ofs.close();
  }
};
