#include "../common/common_sqlite3.c"
#include "os_server_wrap.h"
#include "../common/os_arg_convert.c"

void setClientRemotePMethods(sqlite3_file *pf)   //recive on client: ->return
{
    char NotOnClient = 0;
    assert(NotOnClient);
}

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
#if SQLITE_ENABLE_LOCKING_STYLE && defined(__APPLE__)
        UNIXVFS("unix",          autolockIoFinder ),
#elif OS_VXWORKS
        UNIXVFS("unix",          vxworksIoFinder ),
#else
        UNIXVFS("unix", posixIoFinder),
#endif
        UNIXVFS("unix-none", nolockIoFinder),
        UNIXVFS("unix-dotfile", dotlockIoFinder),
        UNIXVFS("unix-excl", posixIoFinder),
#if OS_VXWORKS
        UNIXVFS("unix-namedsem", semIoFinder ),
#endif
#if SQLITE_ENABLE_LOCKING_STYLE || OS_VXWORKS
        UNIXVFS("unix-posix",    posixIoFinder ),
#endif
#if SQLITE_ENABLE_LOCKING_STYLE
        UNIXVFS("unix-flock",    flockIoFinder ),
#endif
#if SQLITE_ENABLE_LOCKING_STYLE && defined(__APPLE__)
UNIXVFS("unix-afp",      afpIoFinder ),
    UNIXVFS("unix-nfs",      nfsIoFinder ),
    UNIXVFS("unix-proxy",    proxyIoFinder ),
#endif
};

void WrapInit(const char *argIn, char *argOut) {
    printf("---WrapInit:\n");
    int rc = sqlite3_os_init();
    sqlite3_os_initConvertReturnToChar(&rc, argOut);
}

void WrapOpen(const char *argIn, char *argOut) {
    printf("---WrapOpen:\n");

    char path[512];
    unixFile file_infor;
    sqlite3_file *pId = (sqlite3_file *) &file_infor;
    int in_flags;
    int out_flags;
    int rc = 0;

    unixOpenConvertCharToArgIn(argIn, &aVfs[0], path, pId, &in_flags, &out_flags);
    getServerUnixPMethods(file_infor.h, pId);
//    printf("   id.h = %d,  method = %s\n", file_infor.h,
//           &posixIoMethods == pId->pMethods ? "posixIoMethods" : "otherMethods");
    printf("   path = %s\n", path);
    if (-1 == out_flags) { rc = aVfs[0].xOpen(&aVfs[0], path, pId, in_flags & 0x87f7f, NULL); }
    else { rc = aVfs[0].xOpen(&aVfs[0], path, pId, in_flags & 0x87f7f, &out_flags); }
    setServerUnixPMethods(file_infor.h, pId);
//    printf("   id.h = %d,  method = %s\n", file_infor.h,
//           &posixIoMethods == pId->pMethods ? "posixIoMethods" : "otherMethods");
    unixOpenConvertReturnToChar(pId, &out_flags, &rc, argOut);

}

void WrapDelete(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    char path[512];
    int dirSync;
    unixDeleteConvertCharToArgIn(argIn, &aVfs[0], path, &dirSync);
    int rc = unixDelete(&aVfs[0], path, dirSync);
    unixDeleteConvertReturnToChar(&rc, argOut);
}

void WrapAccess(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    char path[512];
    int flags;
    int res;
    unixAccessConvertCharToArgIn(argIn, &aVfs[0], path, &flags, &res);
    int rc = unixAccess(&aVfs[0], path, flags, &res);
    unixAccessConvertReturnToChar(&res, &rc, argOut);

}

void WrapFullPathname(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

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
    printf("--WrapFileControl:\n");

    int NotUsed2;
    char NotUsed3[2];
    unixGetLastErrorConvertCharToArgIn(argIn, &aVfs[0], &NotUsed2, NotUsed3);
    int rc_errno = unixGetLastError(&aVfs[0], NotUsed2, NotUsed3);
    unixGetLastErrorConvertReturnToChar(&rc_errno, argOut);
}

void WrapWrite(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;

    unixWriteConvertCharToArgIn(argIn, pId, pBuf, &amt, &offset);
    getServerUnixPMethods(id.h, pId);
    int rc = unixWrite(pId, pBuf, amt, offset);
    setServerUnixPMethods(id.h, pId);
    unixWriteConvertReturnToChar(pId, pBuf, &amt, &rc, argOut);


}

void WrapRead(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;

    unixReadConvertCharToArgIn(argIn, pId, pBuf, &amt, &offset);
    getServerUnixPMethods(id.h, pId);
    int rc = unixRead(pId, pBuf, amt, offset);
    setServerUnixPMethods(id.h, pId);
    unixReadConvertReturnToChar(pId, pBuf, &amt, &rc, argOut);
}

void WrapTruncate(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    i64 nByte;

    unixTruncateConvertCharToArgIn(argIn, pId, &nByte);
    getServerUnixPMethods(id.h, pId);
    int rc = unixTruncate(pId, nByte);
    setServerUnixPMethods(id.h, pId);
    unixTruncateConvertReturnToChar(pId, &rc, argOut);
}

void WrapSync(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int flags;

    unixSyncConvertCharToArgIn(argIn, pId, &flags);
    getServerUnixPMethods(id.h, pId);
    int rc = unixSync(pId, flags);
    setServerUnixPMethods(id.h, pId);
    unixSyncConvertReturnToChar(pId, &rc, argOut);
}

void WrapFileSize(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    i64 pSize;

    unixFileSizeConvertCharToArgIn(argIn, pId, &pSize);
    getServerUnixPMethods(id.h, pId);
    int rc = unixFileSize(pId, &pSize);
    setServerUnixPMethods(id.h, pId);
    unixFileSizeConvertReturnToChar(pId, &pSize, &rc, argOut);
}

// TODO: when op == SQLITE_FCNTL_HAS_MOVED, change common_sqlite3.c funtion unixFileControl pArg = 0;
void WrapFileControl(const char *argIn, char *argOut) {
    printf("--WrapFileControl:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int op;
    char pArg[512];
    int size;
    int rc;

    unixFileControlConvertCharToArgIn(argIn, pId, &op, pArg, &size);
    getServerUnixPMethods(id.h, pId);
    if (size > 0) { rc = unixFileControl(pId, op, &pArg); }
    else { rc = unixFileControl(pId, op, 0); }
    setServerUnixPMethods(id.h, pId);
    size = LenFileControlPArg(op, pArg);
    unixFileControlConvertReturnToChar(pId, pArg, &rc, argOut, op, &size);
}

void WrapSectorSize(const char *argIn, char *argOut) {
    printf("---WrapOpen:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;

    unixSectorSizeConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    int sectorSize = unixSectorSize(pId);
    setServerUnixPMethods(id.h, pId);
    unixSectorSizeConvertReturnToChar(pId, &sectorSize, argOut);
}

void WrapDeviceCharacteristics(const char *argIn, char *argOut) {
    printf("---WrapOpen:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;

    unixDeviceCharacteristicsConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    int deviceCharacteristics = unixDeviceCharacteristics(pId);
    setServerUnixPMethods(id.h, pId);
    unixDeviceCharacteristicsConvertReturnToChar(pId, &deviceCharacteristics, argOut);
}

void WrapClose(const char *argIn, char *argOut) {
    printf("---WrapClose:\n");
    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int rc;

    unixCloseConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    if (pId->pMethods) {
        rc = pId->pMethods->xClose(pId);
        pId->pMethods = 0;
    }
    setServerUnixPMethods(id.h, pId);
    unixCloseConvertReturnToChar(pId, &rc, argOut);
}

void WrapLock(const char *argIn, char *argOut) {
    printf("---WrapLock:\n");
    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int eFileLock;

    unixLockConvertCharToArgIn(argIn, pId, &eFileLock);
    getServerUnixPMethods(id.h, pId);
    int rc = unixLock(pId, eFileLock);
    setServerUnixPMethods(id.h, pId);
    unixLockConvertReturnToChar(pId, &rc, argOut);
}

void WrapUnlock(const char *argIn, char *argOut) {
    printf("---WrapUnlock:\n");
    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int eFileLock;

    unixUnlockConvertCharToArgIn(argIn, pId, &eFileLock);
    getServerUnixPMethods(id.h, pId);
    int rc = unixUnlock(pId, eFileLock);
    setServerUnixPMethods(id.h, pId);
    unixUnlockConvertReturnToChar(pId, &rc, argOut);
}

void WrapCheckReservedLock(const char *argIn, char *argOut) {
    printf("---WrapCheckReservedLock:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    int pResOut = 0;

    unixCheckReservedLockConvertCharToArgIn(argIn, pId, &pResOut);
    getServerUnixPMethods(id.h, pId);
    int rc = unixCheckReservedLock(pId, &pResOut);
    setServerUnixPMethods(id.h, pId);
    unixCheckReservedLockConvertReturnToChar(pId, &pResOut, &rc, argOut);
}

void WrapFetch(const char *argIn, char *argOut) {
    printf("---WrapFetch:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    i64 iOff;
    int nAmt;
    void *pData = 0;

    unixFetchConvertCharToArgIn(argIn, pId, &iOff, &nAmt, &pData);
    getServerUnixPMethods(id.h, pId);
    int rc = unixFetch(pId, iOff, nAmt, &pData);
    setServerUnixPMethods(id.h, pId);
    unixFetchsConvertReturnToChar(pId, (char *) pData, nAmt, &rc, argOut);
}

void WrapUnfetch(const char *argIn, char *argOut) {
    printf("---WrapUnfetch:\n");

    unixFile id;
    sqlite3_file *pId = (sqlite3_file *) &id;
    i64 iOff;
    int p_flag;

    unixUnfetchConvertCharToArgIn(argIn, pId, &iOff, &p_flag);
    getServerUnixPMethods(id.h, pId);
    void *pData = (p_flag == 0 ? 0 : &id);
    int rc = unixUnfetch(pId, iOff, pData);
    setServerUnixPMethods(id.h, pId);
    unixUnfetchConvertReturnToChar(pId, &rc, argOut);
}