#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "../common/os_remote.grpc.pb.h"

extern "C" {
#include "os_server_wrap.h"
}

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using os_remote::OSRemote;

class OSRemoteServiceImpl final : public OSRemote::Service {
    Status Init(ServerContext *context, const os_remote::ArgRequest *request,
                os_remote::ArgReply *reply) override {

        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapInit(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }


    Status Open(ServerContext *context, const os_remote::ArgRequest *request,
                os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapOpen(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Delete(ServerContext *context, const os_remote::ArgRequest *request,
                  os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapDelete(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Access(ServerContext *context, const os_remote::ArgRequest *request,
                  os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapAccess(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status FullPathname(ServerContext *context, const os_remote::ArgRequest *request,
                        os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapFullPathname(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }
    /*
 * 以下几个函数直接调用客户端函数，不用调用remote：
 *      unixRandomness
 *      unixSleep
 *      unixCurrentTime
 *      unixCurrentTimeInt64
 * 所以注释掉了
 */
/*
    Status Randomness(ServerContext *context, const os_remote::ArgRequest *request,
                      os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapRandomness(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Sleep(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapSleep(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status CurrentTime(ServerContext *context, const os_remote::ArgRequest *request,
                       os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapCurrentTime(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status CurrentTimeInt64(ServerContext *context, const os_remote::ArgRequest *request,
                            os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapCurrentTimeInt64(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }
    */

    Status GetLastError(ServerContext *context, const os_remote::ArgRequest *request,
                        os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapGetLastError(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Write(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapWrite(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Read(ServerContext *context, const os_remote::ArgRequest *request,
                os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapRead(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }


    Status Truncate(ServerContext *context, const os_remote::ArgRequest *request,
                    os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapTruncate(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Sync(ServerContext *context, const os_remote::ArgRequest *request,
                os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapSync(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status FileSize(ServerContext *context, const os_remote::ArgRequest *request,
                    os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapFileSize(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status FileControl(ServerContext *context, const os_remote::ArgRequest *request,
                       os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapFileControl(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status SectorSize(ServerContext *context, const os_remote::ArgRequest *request,
                      os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapSectorSize(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status DeviceCharacteristics(ServerContext *context, const os_remote::ArgRequest *request,
                                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapDeviceCharacteristics(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Close(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapClose(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Lock(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapLock(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Unlock(ServerContext *context, const os_remote::ArgRequest *request,
                 os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapUnlock(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status CheckReservedLock(ServerContext *context, const os_remote::ArgRequest *request,
                  os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapCheckReservedLock(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }

    Status Fetch(ServerContext *context, const os_remote::ArgRequest *request,
                             os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapFetch(in, out_struct);

        reply->set_outarg(out_struct, out_size);
        return Status::OK;
    }
    Status Unfetch(ServerContext *context, const os_remote::ArgRequest *request,
                             os_remote::ArgReply *reply) override {
        int out_size = request->outlen();
        const char *in = request->inarg().data();
        char out_struct[out_size];
        WrapUnfetch(in, out_struct);

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