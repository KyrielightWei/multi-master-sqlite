#include "os_server_wrap.h"
#include "../common/os_arg_convert.c"

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
    unixFile file_infor;
    int in_flags;
    int out_flags;

    unixOpenConvertCharToArgIn(argIn, &aVfs[0], path, (sqlite3_file *) &file_infor, &in_flags, &out_flags);
    int rc = unixOpen(&aVfs[0], path, (sqlite3_file *) &file_infor, in_flags, &out_flags);
    unixOpenConvertReturnToChar((sqlite3_file *) &file_infor, &out_flags, &rc, argOut);
}

void WrapDelete(const char *argIn, char *argOut) {
    char path[512];
    int dirSync;
    unixDeleteConvertCharToArgIn(argIn, &aVfs[0], path, &dirSync);
    int rc = unixDelete(&aVfs[0], path, dirSync);
    unixDeleteConvertReturnToChar(&rc, argOut);
}

void WrapAccess(const char *argIn, char *argOut) {
    char path[512];
    int flags;
    int res;
    unixAccessConvertCharToArgIn(argIn, &aVfs[0], path, &flags, &res);
    int rc = unixAccess(&aVfs[0], path, flags, &res);
    unixAccessConvertReturnToChar(&res, &rc, argOut);

}

void WrapFullPathname(const char *argIn, char *argOut) {
    char path[512];
    int nOut;
    char zOut[512];

    unixFullPathnameConvertCharToArgIn(argIn, &aVfs[0], path, &nOut, zOut);
    int rc = unixFullPathname(&aVfs[0], path, nOut, zOut);
    unixFullPathnameConvertReturnToChar(zOut, &rc, argOut);
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
void WrapRandomness(const char *argIn, char *argOut) {
    printf("---WrapRandomness:\n");
    int nBuf;
    char zBuf[256];
    unixRandomnessConvertCharToArgIn(argIn, &aVfs[0], &nBuf, zBuf);
    int rc = unixRandomness(&aVfs[0], nBuf, zBuf);
    printf("   rc = %d\n", rc);
    unixRandomnessConvertReturnToChar(zBuf, &rc, argOut);
}

void WrapSleep(const char *argIn, char *argOut) {
    int nMicro;
    unixSleepConvertCharToArgIn(argIn, &aVfs[0], &nMicro);
    int rc = unixSleep(&aVfs[0], nMicro);
    unixSleepConvertReturnToChar(&rc, argOut);
}

void WrapCurrentTime(const char *argIn, char *argOut) {
    double prNow;
    unixCurrentTimeConvertCharToArgIn(argIn, &aVfs[0], &prNow);
    int rc = unixCurrentTime(&aVfs[0], &prNow);
    unixCurrentTimeConvertReturnToChar(&prNow, &rc, argOut);
}
void WrapCurrentTimeInt64(const char *argIn, char *argOut) {
    sqlite3_int64 piNow;
    unixCurrentTimeInt64ConvertCharToArgIn(argIn, &aVfs[0], &piNow);
    int rc = unixCurrentTimeInt64(&aVfs[0], &piNow);
    unixCurrentTimeInt64ConvertReturnToChar(&piNow, &rc, argOut);
}
*/

void WrapGetLastError(const char *argIn, char *argOut) {
    int NotUsed2;
    char NotUsed3[2];
    unixGetLastErrorConvertCharToArgIn(argIn, &aVfs[0], &NotUsed2, NotUsed3);
    int rc_errno = unixGetLastError(&aVfs[0], NotUsed2, NotUsed3);
    unixGetLastErrorConvertReturnToChar(&rc_errno, argOut);
}
void WrapWrite(const char *argIn, char *argOut) {
    unixFile id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;
    unixWriteConvertCharToArgIn(argIn, (sqlite3_file *) &id, pBuf, &amt, &offset);
    int rc = unixWrite((sqlite3_file *) &id, pBuf, amt, offset);
    unixWriteConvertReturnToChar((sqlite3_file *) &id, pBuf,&amt, &rc, argOut);
}

void WrapRead(const char *argIn, char *argOut) {
    unixFile id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;

    unixReadConvertCharToArgIn(argIn, (sqlite3_file *) &id, pBuf, &amt, &offset);
    int rc = unixRead((sqlite3_file *) &id, pBuf, amt, offset);
    unixReadConvertReturnToChar((sqlite3_file *) &id, pBuf, &amt, &rc, argOut);
}