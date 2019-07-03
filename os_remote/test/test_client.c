/* 
#include <iostream>
#include <memory>
#include <string>

#include<grpc/grpc.h>
#include<grpcpp/grpcpp.h>
#include <stdio.h>
#include "sqlite3.c"
#include "test.grpc.pb.h"
*/

#include "sqlite3.c"

//#include "remote_tool.h"
//extern  client_test();
/* 
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test::Test;
*/

extern void client_test();

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

  client_test();
  return 0;
}
