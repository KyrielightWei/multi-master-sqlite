
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

/***
 *  形如以下形式的cient包装函数，参数应该是char*(所有参数打包的结构体的指针)，在该函数内打包成string传递 
extern "C" void client_test()
{

}
*/