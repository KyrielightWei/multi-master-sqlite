#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "../common/os_remote.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

typedef unsigned int u32;

std::unique_ptr <os_remote::OSRemote::Stub> stub_ = os_remote::OSRemote::NewStub(
        grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));

extern "C" const char *clientInit(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Init(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}


extern "C" const char *clientOpen(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Open(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }

}

extern "C" const char *clientDelete(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Delete(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientAccess(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Access(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}


extern "C" const char *clientFullPathname(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->FullPathname(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().c_str(), outlen);
        return reply.outarg().c_str();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}
/*
extern "C" const char *clientRandomness(char *argin, u32 inlen, u32 outlen) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Randomness(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().data();
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
        return reply.outarg().data();
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
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientCurrentTimeInt64(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->CurrentTimeInt64(&context, request, &reply);

    if (status.ok()) {
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}*/

extern "C" const char *clientGetLastError(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->GetLastError(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientWrite(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Write(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}


extern "C" const char *clientRead(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Read(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientTruncate(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Truncate(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientSync(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Sync(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientFileSize(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->FileSize(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientFileControl(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->FileControl(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientSectorSize(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->SectorSize(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientDeviceCharacteristics(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->DeviceCharacteristics(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientClose(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Close(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}
extern "C" const char *clientLock(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Lock(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}
extern "C" const char *clientUnlock(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Unlock(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientCheckReservedLock(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->CheckReservedLock(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientFetch(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Fetch(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}

extern "C" const char *clientUnfetch(char *argin, u32 inlen, u32 outlen, char *outarg) {
    os_remote::ArgRequest request;
    os_remote::ArgReply reply;
    ClientContext context;

    request.set_inarg(argin, inlen);
    request.set_outlen(outlen);

    Status status = stub_->Unfetch(&context, request, &reply);

    if (status.ok()) {
        memcpy(outarg, reply.outarg().data(), outlen);
        return reply.outarg().data();
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        return "RPC failed";
    }
}
