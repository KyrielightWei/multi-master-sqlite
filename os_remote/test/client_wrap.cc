
#include <iostream>
#include <memory>
#include <string>

#include<grpc/grpc.h>
#include<grpcpp/grpcpp.h>
//#include <stdio.h>
//#include "sqlite3.c"
#include "test.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test::Test;
using test::OpenReply;
using test::OpenSend;

typedef unsigned int u32;


extern "C" const char * client_open(char * argin,u32 inlen,u32 outlen)
{
  std::unique_ptr<Test::Stub> client_stub = Test::NewStub(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  ClientContext context;
  OpenSend send;
  OpenReply reply;
  //std::string in,out;
  
  send.set_inarg(argin,inlen);
  send.set_outlen(outlen);
 // send.set_inlen(inlen);
 // send.set_outlen(outlen);
  Status status = client_stub->TestOpen(&context,send,&reply);

  //memcpy(argout,reply.outarg().c_str(),outlen);
  
  if (status.ok()) {
       std::cout <<"success！" <<std::endl;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        std::cout << "RPC failed";
    }
  
  return reply.outarg().c_str();
}
