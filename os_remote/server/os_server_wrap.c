#include "os_server_wrap.h"
#include "os_arg_convert.c"

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

static sqlite3_vfs aVfs[] = {
        UNIXVFS("unix", posixIoFinder),
};

char *WrapInit(void) {

    int rc = sqlite3_os_init();
    char *p = (char *) (&rc);
    return p;
}

void WrapOpen(const char *argIn, char *argOut) {
    char path[512];
    char file_infor[sizeof(unixFile)];
    int in_flags;
    int out_flags;
    unixOpenConvertCharToArgIn(argIn, &aVfs[0], path, (sqlite3_file *) file_infor, &in_flags, &out_flags);
    int rc = unixOpen(&aVfs[0], path, (sqlite3_file *) file_infor, in_flags, &out_flags);
    unixOpenConvertArgOutToChar((sqlite3_file *) file_infor, &out_flags, &rc, argOut);
    return;
}

void WrapDelete(const char *argIn, char *argOut){
    char path[512];
    int dirSync;
    unixDeleteConvertCharToArgIn(argIn, &aVfs[0], path, &dirSync);
    int rc = unixDelete(&aVfs[0], path,dirSync);
    unixDeleteConvertArgOutToChar(&rc, argOut);
}