LDFLAGS = -L/usr/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: client server

client: file_trans.pb.o file_trans.grpc.pb.o file_trans_client.o
	$(CXX) $^ $(LDFLAGS) -o $@

server: file_trans.pb.o file_trans.grpc.pb.o file_trans_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc-3.13.0.0 --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc-3.13.0.0 --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h client server
