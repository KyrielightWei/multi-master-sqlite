#include "../common/common_sqlite3.c"
#include "os_server_wrap.h"
#include "../common/os_arg_convert.c"
#include "../client/util.c"

#define SERVER_DEBUG_FLAG 0

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
        UNIXVFS("unix", posixIoFinder),
        UNIXVFS("unix-none", nolockIoFinder),

};

void WrapInit(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapInit:\n");
    DebugClient(sprintf(debugStr, "---start WrapInit.        \n"), debugStr);
#endif
    int rc = sqlite3_os_init();
    sqlite3_os_initConvertReturnToChar(&rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapInit.        \n"), debugStr);
#endif
}

void WrapOpen(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapOpen:\n");
    DebugClient(sprintf(debugStr, "---start WrapOpen.        \n"), debugStr);
#endif

    char path[512];
    unixFile file_infor;
    sqlite3_file *pId = (sqlite3_file * ) & file_infor;
    int in_flags;
    int out_flags;
    int rc = 0;

    unixOpenConvertCharToArgIn(argIn, &aVfs[0], path, pId, &in_flags, &out_flags);
    getServerUnixPMethods(file_infor.h, pId);
    getServicePath(file_infor.h, &file_infor);
#if SERVER_DEBUG_FLAG
    printf("   path = %s\n", path);
#endif
    if (-1 == out_flags) { rc = aVfs[0].xOpen(&aVfs[0], path, pId, in_flags & 0x87f7f, NULL); }
    else { rc = aVfs[0].xOpen(&aVfs[0], path, pId, in_flags & 0x87f7f, &out_flags); }
    setServerUnixPMethods(file_infor.h, pId);
    setServicePath(file_infor.h, path);
    unixOpenConvertReturnToChar(pId, &out_flags, &rc, argOut);


#if SERVER_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended WrapOpen. id.h = %d IOmethod = %d path = %s   \n", file_infor.h, pId->pMethods,
                    file_infor.zPath), debugStr);
#endif
}

void WrapDelete(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapDelete:\n");
    DebugClient(sprintf(debugStr, "---start WrapDelete.      \n"), debugStr);
#endif

    char path[512];
    int dirSync;
    unixDeleteConvertCharToArgIn(argIn, &aVfs[0], path, &dirSync);
    int rc = unixDelete(&aVfs[0], path, dirSync);
    unixDeleteConvertReturnToChar(&rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapDelete.      \n"), debugStr);
#endif
}

void WrapAccess(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapAccess:\n");
    DebugClient(sprintf(debugStr, "---start WrapAccess.      \n"), debugStr);
#endif

    char path[512];
    int flags;
    int res;

    unixAccessConvertCharToArgIn(argIn, &aVfs[0], path, &flags, &res);
#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "                    zPath=%s\n", path), debugStr);
#endif
    int rc = unixAccess(&aVfs[0], path, flags, &res);
    unixAccessConvertReturnToChar(&res, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapAccess.      \n"), debugStr);
#endif
}

void WrapFullPathname(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapFullPathname:\n");
    DebugClient(sprintf(debugStr, "---start WrapFullPathname.\n"), debugStr);
#endif

    int nOut;
    nOut = getFullPathname_OUT_LEN(argIn);
    char *path = (char *) malloc(sizeof(char) * nOut);
    char *zOut = (char *) malloc(sizeof(char) * nOut);

    unixFullPathnameConvertCharToArgIn(argIn, &aVfs[0], path, &nOut, zOut);
#if SERVER_DEBUG_FLAG
    printf("FULL path = %s \n\n", path);
#endif
    int rc = unixFullPathname(&aVfs[0], path, nOut, zOut);
#if SERVER_DEBUG_FLAG
    printf("FULL zOut = %s \n\n", zOut);
#endif
    unixFullPathnameConvertReturnToChar(nOut, zOut, &rc, argOut);
#if SERVER_DEBUG_FLAG
    printf("FULL argOut = %s \n\n", argOut);
#endif
    free(path);
    free(zOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapFullPathname.\n"), debugStr);
#endif
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
#if SERVER_DEBUG_FLAG
    printf("--WrapGetLastError:\n");
    DebugClient(sprintf(debugStr, "---start WrapGetLastError.\n"), debugStr);
#endif

    int NotUsed2;
    char NotUsed3[2];
    unixGetLastErrorConvertCharToArgIn(argIn, &aVfs[0], &NotUsed2, NotUsed3);
    int rc_errno = unixGetLastError(&aVfs[0], NotUsed2, NotUsed3);
    unixGetLastErrorConvertReturnToChar(&rc_errno, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapGetLastError.\n"), debugStr);
#endif
}

void WrapWrite(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapWrite:\n");
    DebugClient(sprintf(debugStr, "---start WrapWrite.       \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;

    unixWriteConvertCharToArgIn(argIn, pId, pBuf, &amt, &offset);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixWrite(pId, pBuf, amt, offset);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixWriteConvertReturnToChar(pId, pBuf, &amt, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapWrite.       \n"), debugStr);
#endif
}

void WrapRead(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapRead:\n");
    DebugClient(sprintf(debugStr, "---start WrapRead.        \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;

    unixReadConvertCharToArgIn(argIn, pId, pBuf, &amt, &offset);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixRead(pId, pBuf, amt, offset);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixReadConvertReturnToChar(pId, pBuf, &amt, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapRead.        \n"), debugStr);
#endif
}

void WrapTruncate(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapTruncate:\n");
    DebugClient(sprintf(debugStr, "---start WrapTruncate.    \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    i64 nByte;

    unixTruncateConvertCharToArgIn(argIn, pId, &nByte);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixTruncate(pId, nByte);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixTruncateConvertReturnToChar(pId, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapTruncate.    \n"), debugStr);
#endif
}

void WrapSync(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapSync:\n");
    DebugClient(sprintf(debugStr, "---start WrapSync.        \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int flags;

    unixSyncConvertCharToArgIn(argIn, pId, &flags);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixSync(pId, flags);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixSyncConvertReturnToChar(pId, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapSync.        \n"), debugStr);
#endif
}

void WrapFileSize(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapFileSize:\n");
    DebugClient(sprintf(debugStr, "---start WrapFileSize.    \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    i64 pSize;

    unixFileSizeConvertCharToArgIn(argIn, pId, &pSize);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixFileSize(pId, &pSize);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixFileSizeConvertReturnToChar(pId, &pSize, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "                         fd=%d, size=%d, rc=%d\n", id.h, pSize, rc), debugStr);
    DebugClient(sprintf(debugStr, "---ended WrapFileSize.    \n"), debugStr);
#endif
}

// TODO: when op == SQLITE_FCNTL_HAS_MOVED, change common_sqlite3.c funtion unixFileControl pArg = 0;
void WrapFileControl(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("--WrapFileControl:\n");
    DebugClient(sprintf(debugStr, "---start WrapFileControl. \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int op;
    char pArg[512];
    int size;
    int rc;

    unixFileControlConvertCharToArgIn(argIn, pId, &op, pArg, &size);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    if (size > 0) { rc = unixFileControl(pId, op, &pArg); }
    else { rc = unixFileControl(pId, op, 0); }
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    size = LenFileControlPArg(op, pArg);
    unixFileControlConvertReturnToChar(pId, pArg, &rc, argOut, op, &size);

#if SERVER_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended WrapFileControl      :id.h=%d , path=%s, path_address=%d\n", id.h, id.zPath,
                    id.zPath), debugStr);
#endif
}

void WrapSectorSize(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapSectorSize:\n");
    DebugClient(sprintf(debugStr, "---start WrapSectorSize.  \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;

    unixSectorSizeConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int sectorSize = unixSectorSize(pId);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixSectorSizeConvertReturnToChar(pId, &sectorSize, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapSectorSize.  \n"), debugStr);
#endif
}

void WrapDeviceCharacteristics(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapDeviceCharacteristics:\n");
    DebugClient(sprintf(debugStr, "---start WrapDeviceCharacteristics.\n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;

    unixDeviceCharacteristicsConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
#if SERVER_DEBUG_FLAG
    printf("                      : path=%s\n", id.zPath);
#endif
    int deviceCharacteristics = unixDeviceCharacteristics(pId);
#if SERVER_DEBUG_FLAG
    printf("                      : path=%s\n", id.zPath);
#endif
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixDeviceCharacteristicsConvertReturnToChar(pId, &deviceCharacteristics, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapDeviceCharacteristics: id.h=%d , path=%s, path_address=%d\n", id.h, id.zPath,
                        id.zPath), debugStr);
#endif
}

void WrapClose(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapClose: ");
    DebugClient(sprintf(debugStr, "---start WrapClose.       \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int rc;

    unixCloseConvertCharToArgIn(argIn, pId);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);

#if SERVER_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---Middle WrapClose.  id.h=%d, unixclose=%d, nolockIoMethods=%d, PMethods=%d \n", id.h,
                    &posixIoMethods, &nolockIoMethods, pId->pMethods), debugStr);
#endif
    if (pId->pMethods) {
        rc = pId->pMethods->xClose(pId);
        pId->pMethods = 0;
    }
    unixCloseConvertReturnToChar(pId, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapClose.       \n"), debugStr);
#endif
}

void WrapLock(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapLock:\n");
    DebugClient(sprintf(debugStr, "---start WrapLock.        \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int eFileLock;

    unixLockConvertCharToArgIn(argIn, pId, &eFileLock);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixLock(pId, eFileLock);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixLockConvertReturnToChar(pId, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapLock.        \n"), debugStr);
#endif
}

void WrapUnlock(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapUnlock:\n");
    DebugClient(sprintf(debugStr, "---start WrapUnlock.      \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int eFileLock;

    unixUnlockConvertCharToArgIn(argIn, pId, &eFileLock);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixUnlock(pId, eFileLock);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixUnlockConvertReturnToChar(pId, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapUnlock.      \n"), debugStr);
#endif
}

void WrapCheckReservedLock(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapCheckReservedLock:\n");
    DebugClient(sprintf(debugStr, "---start WrapCheckReservedLock.\n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    int pResOut = 0;

    unixCheckReservedLockConvertCharToArgIn(argIn, pId, &pResOut);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixCheckReservedLock(pId, &pResOut);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixCheckReservedLockConvertReturnToChar(pId, &pResOut, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapCheckReservedLock.\n"), debugStr);
#endif
}

void WrapFetch(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapFetch:\n");
    DebugClient(sprintf(debugStr, "---start WrapFetch.       \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    i64 iOff;
    int nAmt;
    void *pData = 0;

    unixFetchConvertCharToArgIn(argIn, pId, &iOff, &nAmt, &pData);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    int rc = unixFetch(pId, iOff, nAmt, &pData);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixFetchsConvertReturnToChar(pId, (char *) pData, nAmt, &rc, argOut);

#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapFetch.       \n"), debugStr);
#endif
}

void WrapUnfetch(const char *argIn, char *argOut) {
#if SERVER_DEBUG_FLAG
    printf("---WrapUnfetch:\n");
    DebugClient(sprintf(debugStr, "---start WrapUnfetch.     \n"), debugStr);
#endif

    unixFile id;
    sqlite3_file *pId = (sqlite3_file * ) & id;
    i64 iOff;
    int p_flag;

    unixUnfetchConvertCharToArgIn(argIn, pId, &iOff, &p_flag);
    getServerUnixPMethods(id.h, pId);
    getServicePath(id.h, &id);
    void *pData = (p_flag == 0 ? 0 : &id);
    int rc = unixUnfetch(pId, iOff, pData);
    setServerUnixPMethods(id.h, pId);
    setServicePath(id.h, id.zPath);
    unixUnfetchConvertReturnToChar(pId, &rc, argOut);
#if SERVER_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended WrapUnfetch.     \n"), debugStr);
#endif
}