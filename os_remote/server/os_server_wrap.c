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

void WrapDelete(const char *argIn, char *argOut) {
    char path[512];
    int dirSync;
    unixDeleteConvertCharToArgIn(argIn, &aVfs[0], path, &dirSync);
    int rc = unixDelete(&aVfs[0], path, dirSync);
    unixDeleteConvertArgOutToChar(&rc, argOut);
}

void WrapAccess(const char *argIn, char *argOut) {
    char path[512];
    int flags;
    int res;
    unixAccessConvertCharToArgIn(argIn, &aVfs[0], path, &flags, &res);
    int rc = unixAccess(&aVfs[0], path, flags, &res);
    unixAccessConvertArgOutToChar(&res, &rc, argOut);

}

void WrapFullPathname(const char *argIn, char *argOut) {
    char path[512];
    int nOut;
    char zOut[512];

    unixFullPathnameConvertCharToArgIn(argIn, &aVfs[0], path, &nOut, zOut);
    int rc = unixFullPathname(&aVfs[0], path, nOut, zOut);
    unixFullPathnameConvertArgOutToChar(zOut, &rc, argOut);
}

void WrapRandomness(const char *argIn, char *argOut) {
    int nBuf;
    char zBuf[256];
    unixRandomnessConvertCharToArgIn(argIn, &aVfs[0], &nBuf, zBuf);
    int rc = unixRandomness(&aVfs[0], nBuf, zBuf);
    unixRandomnessConvertArgOutToChar(zBuf, &rc, argOut);
}

void WrapSleep(const char *argIn, char *argOut) {
    int nMicro;
    unixSleepConvertCharToArgIn(argIn, &aVfs[0], &nMicro);
    int rc = unixSleep(&aVfs[0], nMicro);
    unixSleepConvertArgOutToChar(&rc, argOut);
}

void WrapCurrentTime(const char *argIn, char *argOut) {
    double prNow;
    unixCurrentTimeConvertCharToArgIn(argIn, &aVfs[0], &prNow);
    int rc = unixCurrentTime(&aVfs[0], &prNow);
    unixCurrentTimeConvertArgOutToChar(&prNow, &rc, argOut);
}

void WrapGetLastError(const char *argIn, char *argOut) {
    int NotUsed2;
    char NotUsed3[2];
    unixGetLastErrorConvertCharToArgIn(argIn, &aVfs[0], &NotUsed2, NotUsed3);
    int rc_errno = unixGetLastError(&aVfs[0], NotUsed2, NotUsed3);
    unixGetLastErrorConvertArgOutToChar(&rc_errno, argOut);
}

void WrapCurrentTimeInt64(const char *argIn, char *argOut) {
    sqlite3_int64 piNow;
    unixCurrentTimeInt64ConvertCharToArgIn(argIn, &aVfs[0], &piNow);
    int rc = unixCurrentTimeInt64(&aVfs[0], &piNow);
    unixCurrentTimeInt64ConvertArgOutToChar(&piNow, &rc, argOut);
}

void WrapWrite(const char *argIn, char *argOut) {
    char id[sizeof(unixFile)];
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;
    unixWriteConvertCharToArgIn(argIn, (sqlite3_file *)id, pBuf, &amt, &offset);
    int rc = unixWrite((sqlite3_file *)id, pBuf, amt, offset);
    unixWriteConvertArgOutToChar((sqlite3_file *)id, pBuf, &rc, argOut);
}
