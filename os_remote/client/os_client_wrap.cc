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

std::unique_ptr <os_remote::OSRemote::Stub> stub_ = os_remote::OSRemote::NewStub(
        grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));

extern "C" const char *clientInit() {
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


extern "C" const char *clientOpen(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Open(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }

}

extern "C" const char *clientDelete(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Delete(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientAccess(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Access(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientFullPathname(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->FullPathname(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientRandomness(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Randomness(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientSleep(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Sleep(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientCurrentTime(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->CurrentTime(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}
extern "C" const char *clientGetLastError(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->GetLastError(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientCurrentTimeInt64(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->CurrentTimeInt64(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientWrite(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Write(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}