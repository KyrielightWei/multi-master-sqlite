#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
// for C
#include <stdlib.h>
#include <string.h>

#include "os_remote.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

typedef unsigned int u32;

std::unique_ptr<os_remote::OSRemote::Stub> stub_ = os_remote::OSRemote::NewStub(
        grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));

extern "C" const char *ClientInit() {
    os_remote::InitRequest request;
    os_remote::InitReply reply;
    ClientContext context;

    Status status = stub_->Init(&context, request, &reply);

    if (status.ok()) {
        return reply.rc().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}


extern "C" const char *ClientOpen(char * argin,u32 inlen,u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin,inlen);
    request.set_outlen(outlen);

    Status status = stub_->Open(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }

}

extern "C" const char *ClientDelete(char *argin, u32 inlen, u32 outlen){
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin,inlen);
    request.set_outlen(outlen);

    Status status = stub_->Delete(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}