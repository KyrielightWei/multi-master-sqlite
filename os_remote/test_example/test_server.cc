//#include "sqlite3.h"

/*gcc test.c -o test -pthread -ldl */
//#include "sqlite3.c"
extern "C"
{
  #include "server_wrap.h"
}
#include "test.grpc.pb.h"
//#include <stdio.h>

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using test::Test;
using test::OpenReply;
using test::OpenSend;



class TestServiceImpl : public Test::Service{
  Status TestOpen(ServerContext* context, const OpenSend* request, OpenReply* response) override
  {
    //ConvertStringToBytes(request->file_infor,*fi,sizeof(unixFile));
    int out_size = request->outlen();
    const char * in = request->inarg().c_str();
    char out_struct[out_size];
    wrapOpen(in,out_struct);

    response->set_outarg(out_struct,out_size);
    return Status::OK;
  }
};


int main() {
  std::string server_address("0.0.0.0:50051");

  TestServiceImpl service;
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}