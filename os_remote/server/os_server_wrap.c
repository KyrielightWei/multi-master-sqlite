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

void WrapInit(const char *argIn, char *argOut){
    printf("---WrapInit:\n");
    int rc = sqlite3_os_init();
    sqlite3_os_initConvertReturnToChar(&rc, argOut);
}

void WrapOpen(const char *argIn, char *argOut) {
    printf("---WrapOpen:\n");
    char path[512];
    unixFile file_infor;
    int in_flags;
    int out_flags;

    unixOpenConvertCharToArgIn(argIn, &aVfs[0], path, (sqlite3_file *) &file_infor, &in_flags, &out_flags);
    printf("   id.h = %d\n", file_infor.h);
    int rc = unixOpen(&aVfs[0], path, (sqlite3_file *) &file_infor, in_flags, &out_flags);
    printf("   id.h = %d\n", file_infor.h);
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
    unixWriteConvertReturnToChar((sqlite3_file *) &id, pBuf, &amt, &rc, argOut);
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

void WrapTruncate(const char *argIn, char *argOut) {
    unixFile id;
    i64 nByte;

    unixTruncateConvertCharToArgIn(argIn, (sqlite3_file *) &id, &nByte);
    int rc = unixTruncate((sqlite3_file *) &id, nByte);
    unixTruncateConvertReturnToChar((sqlite3_file *) &id, &rc, argOut);
}
void WrapSync(const char *argIn, char *argOut) {
    unixFile id;
    int flags;

    unixSyncConvertCharToArgIn(argIn, (sqlite3_file *) &id, &flags);
    int rc = unixSync((sqlite3_file *) &id, flags);
    unixSyncConvertReturnToChar((sqlite3_file *) &id, &rc, argOut);
}
void WrapFileSize(const char *argIn, char *argOut) {
    unixFile id;
    i64 pSize;

    unixFileSizeConvertCharToArgIn(argIn, (sqlite3_file *) &id, &pSize);
    int rc = unixFileSize((sqlite3_file *) &id, &pSize);
    unixFileSizeConvertReturnToChar((sqlite3_file *) &id, &pSize, &rc, argOut);
}
void WrapFileControl(const char *argIn, char *argOut) {
    unixFile id;
    int op;
    int pArg;

    unixFileControlConvertCharToArgIn(argIn, (sqlite3_file *) &id, &op, &pArg);
    int rc = unixFileControl((sqlite3_file *) &id, op, &pArg);
    unixFileControlConvertReturnToChar((sqlite3_file *) &id, &pArg, &rc, argOut);
}
void WrapSectorSize(const char *argIn, char *argOut) {
    unixFile id;

    unixSectorSizeConvertCharToArgIn(argIn, (sqlite3_file *) &id);
    int sectorSize = unixSectorSize((sqlite3_file *) &id);
    unixSectorSizeConvertReturnToChar((sqlite3_file *) &id, &sectorSize, argOut);
}
void WrapDeviceCharacteristics(const char *argIn, char *argOut) {
    unixFile id;

    unixDeviceCharacteristicsConvertCharToArgIn(argIn, (sqlite3_file *) &id);
    int deviceCharacteristics = unixDeviceCharacteristics((sqlite3_file *) &id);
    unixDeviceCharacteristicsConvertReturnToChar((sqlite3_file *) &id, &deviceCharacteristics, argOut);
}
void WrapClose(const char *argIn, char *argOut) {
    printf("---WrapClose:\n");
    unixFile id;

    unixCloseConvertCharToArgIn(argIn, (sqlite3_file *) &id);
    printf("   id.h = %d\n", id.h);
    int rc = unixClose((sqlite3_file *) &id);   // TODO: 上面一个能输出，下面一个输出不了，出现了段错误
    printf("   id.h = %d\n", id.h);
    unixCloseConvertReturnToChar((sqlite3_file *) &id, &rc, argOut);
}
void WrapLock(const char *argIn, char *argOut) {
    printf("---WrapLock:\n");
    unixFile id;
    int eFileLock;

    unixLockConvertCharToArgIn(argIn, (sqlite3_file *) &id, &eFileLock);
    printf("   id.h = %d\n", id.h);
    int rc = unixLock((sqlite3_file *) &id, eFileLock);
    printf("   id.h = %d\n", id.h);
    unixLockConvertReturnToChar((sqlite3_file *) &id, &rc, argOut);
}
void WrapUnlock(const char *argIn, char *argOut) {
    printf("---WrapUnlock:\n");
    unixFile id;
    int eFileLock;

    unixUnlockConvertCharToArgIn(argIn, (sqlite3_file *) &id, &eFileLock);
    printf("   id.h = %d\n", id.h);
    int rc = unixUnlock((sqlite3_file *) &id, eFileLock);
    printf("   id.h = %d\n", id.h);
    unixUnlockConvertReturnToChar((sqlite3_file *) &id, &rc, argOut);
}
void WrapCheckReservedLock(const char *argIn, char *argOut) {
    printf("---WrapCheckReservedLock:\n");
    unixFile id;
    int pResOut = 0;

    unixCheckReservedLockConvertCharToArgIn(argIn, (sqlite3_file *) &id, &pResOut);
    printf("   id.h = %d\n", id.h);
    int rc = unixCheckReservedLock((sqlite3_file *) &id, &pResOut);
    printf("   id.h = %d\n", id.h);
    unixCheckReservedLockConvertReturnToChar((sqlite3_file *) &id, &pResOut, &rc, argOut);
}
