#include <fstream>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include "include/file_trans.grpc.pb.h"
#include "include/file_trans.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;
using file_trans::FileChunk;
using file_trans::Request;
using file_trans::WebpackServer;

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
    ofs.open(filename+"x"/*debug +"x"*/, std::ios::out|std::ios::binary);

    while (reader->Read(&fileChunk))
    {
      const std::string& chunk = fileChunk.chunk();
      std::cout << chunk << std::endl;
      ofs.write(chunk.c_str(), chunk.length());
    }
    ofs.close();
  }

private:
  std::unique_ptr<WebpackServer::Stub> stub_;
};

int main(int argc, char** argv) 
{
  // TODO: Add argc checking
  std::string server_address = argv[1]/*"127.0.0.1:51005"*/;
  std::string filename = argv[2]/*"a.txt"*/;
  WebpackServerClient client(grpc::CreateChannel(server_address, 
            		     grpc::InsecureChannelCredentials()));
  client.SendFile(filename);
}
