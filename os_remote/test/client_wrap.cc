
#include "remote_tool.h"


#include <iostream>
#include <memory>
#include <string>

#include<grpc/grpc.h>
#include<grpcpp/grpcpp.h>
#include <stdio.h>
//#include "sqlite3.c"
#include "test.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test::Test;
using test::OpenReply;
using test::OpenSend;


extern "C" void client_test()
{
  std::unique_ptr<Test::Stub> client_stub = Test::NewStub(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  ClientContext context;
  OpenSend send;
  OpenReply reply;
  send.set_vfs_name("unix");
  send.set_f_path("./test.client.c");
  std::string infor = "";
  send.set_file_infor("");
  send.set_in_flags(0);
  send.set_out_flags(0);

  Status status = client_stub->TestOpen(&context,send,&reply);

  //ConvertStringToBytes()
  if (status.ok()) {
       std::cout << reply.rc()<<std::endl;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        std::cout << "RPC failed";
    }
}
