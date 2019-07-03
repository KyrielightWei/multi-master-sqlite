
#include <iostream>
#include <memory>
#include <string>

#include<grpc/grpc.h>
#include<grpcpp/grpcpp.h>
#include <stdio.h>
#include "sqlite3.c"
#include "test.grpc.pb.h"


//#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test::Test;

int main()
{
  
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

  
  //std::unique_ptr<Test::Stub> stub(Test::NewStub(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())));

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

  client_stub->TestOpen(&context,&send,&reply);

  std::cout << reply.rc()<<std::endl;
  return 0;
}
