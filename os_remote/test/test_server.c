//#include "sqlite3.h"

/*gcc test.c -o test -pthread -ldl */
#include "sqlite3.c"
#include "test.grpc.pb.h"
#include <stdio.h>


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using test;

#define UNIXVFS(VFSNAME, FINDER) {                        \
    3,                    /* iVersion */                    \
    sizeof(unixFile),     /* szOsFile */                    \
    MAX_PATHNAME,         /* mxPathname */                  \
    0,                    /* pNext */                       \
    VFSNAME,              /* zName */                       \
    (void*)&FINDER,       /* pAppData */                    \
    unixOpen,             /* xOpen */                       \
    unixDelete,           /* xDelete */                     \
    unixAccess,           /* xAccess */                     \
    unixFullPathname,     /* xFullPathname */               \
    unixDlOpen,           /* xDlOpen */                     \
    unixDlError,          /* xDlError */                    \
    unixDlSym,            /* xDlSym */                      \
    unixDlClose,          /* xDlClose */                    \
    unixRandomness,       /* xRandomness */                 \
    unixSleep,            /* xSleep */                      \
    unixCurrentTime,      /* xCurrentTime */                \
    unixGetLastError,     /* xGetLastError */               \
    unixCurrentTimeInt64, /* xCurrentTimeInt64 */           \
    unixSetSystemCall,    /* xSetSystemCall */              \
    unixGetSystemCall,    /* xGetSystemCall */              \
    unixNextSystemCall,   /* xNextSystemCall */             \
  }

  /*
  ** All default VFSes for unix are contained in the following array.
  **
  ** Note that the sqlite3_vfs.pNext field of the VFS object is modified
  ** by the SQLite core when the VFS is registered.  So the following
  ** array cannot be const.
  */
  static sqlite3_vfs aVfs[] = {
    UNIXVFS("unix",          posixIoFinder ),
  };

class TestServiceImpl : public Test::Service{
  Status TestOpen(ServerContext* context, const OpenSend* request, OpenReply* response) override
  {
    unixFile fi;
    ConvertStringToBytes(request->file_infor,*fi,sizeof(unixFile));
    int rc = unixOpen(&aVfs[0],request->f_path.c_str(),fi,request->in_flags(),request->out_flags())

    response->set_rc(rc);
    string file_i;
    ConvertBytesToString(*fi,file_i,sizeof(unixFile));
    response->set_file_infor(file_i);
    response->set_out_flags(0);
    return Status::OK;
  }
};


int main() {
  std::string server_address("0.0.0.0:50051")

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