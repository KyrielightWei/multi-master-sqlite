#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "os_remote.grpc.pb.h"

extern "C" {
#include "os_server_wrap.h"
}

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using os_remote::OSRemote;


class OSRemoteServiceImpl final : public OSRemote::Service {
    Status Init(ServerContext *context, const os_remote::InitRequest *request,
                os_remote::InitReply *reply) override {

        char *rc = WrapInit();

        reply->set_rc(rc);
        return Status::OK;
    }


    Status Open(ServerContext *context, const os_remote::ArgRequest *request,
                os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapOpen(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Delete(ServerContext *context, const os_remote::ArgRequest *request,
                  os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapDelete(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Access(ServerContext *context, const os_remote::ArgRequest *request,
                  os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapAccess(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status FullPathname(ServerContext *context, const os_remote::ArgRequest *request,
                        os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapFullPathname(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Randomness(ServerContext *context, const os_remote::ArgRequest *request,
                      os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapRandomness(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Sleep(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapSleep(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status CurrentTime(ServerContext *context, const os_remote::ArgRequest *request,
                       os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapCurrentTime(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status GetLastError(ServerContext *context, const os_remote::ArgRequest *request,
                        os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapGetLastError(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status CurrentTimeInt64(ServerContext *context, const os_remote::ArgRequest *request,
                            os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapCurrentTimeInt64(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Write(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().c_str();
        char out_struct[out_size];
        WrapWrite(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    OSRemoteServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr <Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char **argv) {
    RunServer();

    return 0;
}