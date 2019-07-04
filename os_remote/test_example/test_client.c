
#include "arg_convert.c"

extern const char * client_open(char * argin,u32 inlen,u32 outlen);

/**
 * remoteOpen等函数定义
 */
static int remoteOpen(
  sqlite3_vfs *pVfs,           /* The VFS for which this is the xOpen method */
  const char *zPath,           /* Pathname of file to be opened */
  sqlite3_file *pFile,         /* The file descriptor to be filled in */
  int flags,                   /* Input flags to control the opening */
  int *pOutFlags               /* Output flags returned to SQLite core */
){
  char arginChar[sizeof(ArgInUnixOpen)];
  //const char * argoutChar = NULL;
  //char argoutChar[sizeof(ArgOutUnixOpen)];
  int rc = SQLITE_OK;

  convertUnixOpenToCharIn(pVfs,zPath,pFile,flags,pOutFlags,arginChar);
  const char * argoutChar = client_open(arginChar,sizeof(ArgInUnixOpen),sizeof(ArgOutUnixOpen));
  convertCharOutToUnixOpen(argoutChar,pFile,pOutFlags,&rc);
  
  return rc;
}

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
  unixFile fi;
  int outFlag = 2;
  int rc = remoteOpen(&aVfs[0],"./server_wrap.h",(sqlite3_file *)&fi,0,&outFlag);

  printf("f = %d\n",fi.h);
  printf("rc = %d\n",rc);
  return 0;
}
