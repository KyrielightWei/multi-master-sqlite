#include "server_wrap.h"
#include "arg_convert.c"


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
  ** All default VFSes for un
  ix are contained in the following array.
  **
  ** Note that the sqlite3_vfs.pNext field of the VFS object is modified
  ** by the SQLite core when the VFS is registered.  So the following
  ** array cannot be const.
  */
  static sqlite3_vfs aVfs[] = {
    UNIXVFS("unix",          posixIoFinder ),
  };

void wrapOpen(const char * argin,char * argout)
{
  char path[512];
  char file_infor[sizeof(unixFile)];
  int in_flags;
  int out_flags;
  convertCharInToUnixOpen(&aVfs[0],path,(sqlite3_file *)file_infor,&in_flags,&out_flags,argin);
  int rc = unixOpen(&aVfs[0],path,(sqlite3_file *)file_infor,in_flags,&out_flags);
  convertUnixOpenToCharOut(argout,(sqlite3_file *)file_infor,&out_flags,&rc);
  return;
}