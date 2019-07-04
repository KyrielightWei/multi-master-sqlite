#include "os_remote.grpc.pb.h"
//#include <stdio.h>

//链接c语言函数
extern "C"
{
  #include "os_server_wrap.h"
}

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


// class 服务类

int main(void) {
    //服务监听的main函数
    return 0;
}