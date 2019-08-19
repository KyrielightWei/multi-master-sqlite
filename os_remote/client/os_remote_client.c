#include "../common/os_arg_convert.c"
#include "util.c"

#include "../common/os_remote_debug.h"


extern const char *clientInit(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientOpen(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientDelete(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientAccess(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientFullPathname(char *argin, u32 inlen, u32 outlen, char *outarg);
/*
 * 以下几个函数直接调用客户端函数，不用调用remote：
 *      unixRandomness
 *      unixSleep
 *      unixCurrentTime
 *      unixCurrentTimeInt64
 * 所以注释掉了
 */
/*
extern const char *clientRandomness(char *argin, u32 inlen, u32 outlen);
extern const char *clientSleep(char *argin, u32 inlen, u32 outlen);
extern const char *clientCurrentTime(char *argin, u32 inlen, u32 outlen);
extern const char *clientCurrentTimeInt64(char *argin, u32 inlen, u32 outlen);
 */
extern const char *clientGetLastError(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientWrite(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientRead(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientTruncate(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientSync(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientFileSize(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientFileControl(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientSectorSize(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientDeviceCharacteristics(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientClose(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientLock(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientUnlock(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientCheckReservedLock(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientFetch(char *argin, u32 inlen, u32 outlen, char *outarg);

extern const char *clientUnfetch(char *argin, u32 inlen, u32 outlen, char *outarg);


// #define UNIXVFS(VFSNAME, FINDER) {                        \
//     3,                    /* iVersion */                    \
//     sizeof(unixFile),     /* szOsFile */                    \
//     MAX_PATHNAME,         /* mxPathname */                  \
//     0,                    /* pNext */                       \
//     VFSNAME,              /* zName */                       \
//     (void*)&FINDER,       /* pAppData */                    \
//     unixOpen,             /* xOpen */                       \
//     unixDelete,           /* xDelete */                     \
//     unixAccess,           /* xAccess */                     \
//     unixFullPathname,     /* xFullPathname */               \
//     unixDlOpen,           /* xDlOpen */                     \
//     unixDlError,          /* xDlError */                    \
//     unixDlSym,            /* xDlSym */                      \
//     unixDlClose,          /* xDlClose */                    \
//     unixRandomness,       /* xRandomness */                 \
//     unixSleep,            /* xSleep */                      \
//     unixCurrentTime,      /* xCurrentTime */                \
//     unixGetLastError,     /* xGetLastError */               \
//     unixCurrentTimeInt64, /* xCurrentTimeInt64 */           \
//     unixSetSystemCall,    /* xSetSystemCall */              \
//     unixGetSystemCall,    /* xGetSystemCall */              \
//     unixNextSystemCall,   /* xNextSystemCall */             \
//   }

// static sqlite3_vfs aVfs[] = {
//         UNIXVFS("unix", posixIoFinder),
// };
static int remoteDeviceCharacteristics(sqlite3_file *id);


//////////////// funtion unixOpen ///////////////////////////////////////////
int remote_sqlite3_os_init(void) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remote_sqlite3_os_init: \n"), debugStr);
#endif
    char argInChar[sizeof(ArgInInit)];
    memset(argInChar, 0, sizeof(ArgInInit));
    char argOutChar[sizeof(ReturnInit)];
    memset(argOutChar, 0, sizeof(ReturnInit));
    int rc = SQLITE_OK;

    clientInit(argInChar, sizeof(ArgInInit), sizeof(struct ReturnInit), argOutChar);
    sqlite3_os_initConvertCharToReturn(argOutChar, &rc);
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remote_sqlite3_os_init:\n"), debugStr);
#endif
    return rc;
}

//////////////// funtion unixOpen ///////////////////////////////////////////
static int remoteOpen(
        sqlite3_vfs *pVfs,           /* The VFS for which this is the xOpen method */
        const char *zPath,           /* Pathname of file to be opened */
        sqlite3_file *pFile,         /* The file descriptor to be filled in */
        int flags,                   /* Input flags to control the opening */
        int *pOutFlags               /* Output flags returned to SQLite core */
) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteOpen        :\n"), debugStr);
#endif
    char argInChar[sizeof(ArgInOpen)];
    memset(argInChar, 0, sizeof(ArgInOpen));
    char argOutChar[sizeof(ReturnOpen)];
    memset(argOutChar, 0, sizeof(ReturnOpen));
    int rc = SQLITE_OK;

    unixOpenConvertArgInToChar(pVfs, zPath, pFile, flags, pOutFlags, argInChar);
    clientOpen(argInChar, sizeof(ArgInOpen), sizeof(ReturnOpen), argOutChar);
    unixOpenConvertCharToReturn(argOutChar, pFile, pOutFlags, &rc);
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteOpen        : fd=%d, rc=%d \n", ((unixFile *) pFile)->h, rc),
                debugStr);
#endif
    return rc;
}

//////////////// funtion unixDelete ///////////////////////////////////////////
static int remoteDelete(
        sqlite3_vfs *NotUsed,     /* VFS containing this as the xDelete method */
        const char *zPath,        /* Name of file to be deleted */
        int dirSync             /* Output flags returned to SQLite core */
) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteDelete:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInDelete)];
    memset(argInChar, 0, sizeof(ArgInDelete));
    char argOutChar[sizeof(ReturnDelete)];
    memset(argOutChar, 0, sizeof(ReturnDelete));
    int rc = SQLITE_OK;

    unixDeleteConvertArgInToChar(NotUsed, zPath, dirSync, argInChar);
    clientDelete(argInChar, sizeof(ArgInDelete), sizeof(ReturnDelete), argOutChar);
    unixDeleteConvertCharToReturn(argOutChar, &rc);
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteDelete      : rc=%d, path=%s\n", rc, zPath), debugStr);
#endif
    return rc;
}

//////////////// funtion unixAccess ///////////////////////////////////////////
static int remoteAccess(
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut            /* Write result boolean here */
) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteAccess:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInAccess)];
    memset(argInChar, 0, sizeof(ArgInAccess));
    char argOutChar[sizeof(ReturnAccess)];
    memset(argOutChar, 0, sizeof(ReturnAccess));
    int rc = SQLITE_OK;

    unixAccessConvertArgInToChar(NotUsed, zPath, flags, pResOut, argInChar);
    clientAccess(argInChar, sizeof(ArgInAccess), sizeof(ReturnAccess), argOutChar);
    unixAccessConvertCharToReturn(argOutChar, pResOut, &rc);
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "                     zPath :%s\n", zPath), debugStr);
    DebugClient(sprintf(debugStr, "---ended remoteAccess      : pResult=%d, rc=%d\n", *pResOut, rc), debugStr);
#endif
    return rc;
}

//////////////// funtion unixFullPathname ///////////////////////////////////////////
static int remoteFullPathname(
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        const char *zPath,            /* Possibly relative input path */
        int nOut,                     /* Size of output buffer in bytes */
        char *zOut
) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteFullPathname: InputPath = %s ; nout = %d\n", zPath, nOut), debugStr);
#endif

    int arg_len = getFullPathname_ARG_LEN((nOut));
    int return_len = getFullPathname_RETURN_LEN((nOut));
    char argInChar[arg_len];
    memset(argInChar, 0, arg_len);
    int rc = SQLITE_OK;

    unixFullPathnameConvertArgInToChar(pVfs, zPath, nOut, zOut, argInChar);
    int outl = 0;
    char tempc = -1;
    //const char *argOutChar =NULL;
    char outarg[return_len];
    clientFullPathname(argInChar, arg_len, return_len, outarg);
    // DebugClient(sprintf(debugStr, "---Middle remoteFullPathname: zOut=%s ;zout_len = %d,arg_len = %d,return_len = %d ,outl = %d,tempc= %c \n",argOutChar,strlen(argOutChar),arg_len,return_len,outl,tempc), debugStr);
    // int i=0;
    // for (i = 0; i < outl; i++)
    // {
    //    DebugClient(sprintf(debugStr, "---Middle remoteFullPathname:argOut[%d] = %d  , %d \n",i,argOutChar[i],outarg[i]), debugStr);
    // }
    unixFullPathnameConvertCharToReturn(outarg, nOut, zOut, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended remoteFullPathname: rc=%d ; zOut=%s ;zout_len = %d\n", rc, zOut, strlen(zOut)),
            debugStr);
#endif
    return rc;
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
//////////////// funtion unixRandomness ///////////////////////////////////////////
static int remoteRandomness(sqlite3_vfs *NotUsed, int nBuf, char *zBuf) {
    char argInChar[sizeof(ArgInRandomness)];
    memset(argInChar, 0, sizeof(ArgInRandomness));
    int rc = SQLITE_OK;

    unixRandomnessConvertArgInToChar(NotUsed, nBuf, zBuf, argInChar);
    const char *argOutChar = clientRandomness(argInChar, sizeof(ArgInRandomness), sizeof(ReturnRandomness));
    unixRandomnessConvertCharToReturn(argOutChar, zBuf, &rc);
    return rc;
}
//////////////// funtion unixSleep ///////////////////////////////////////////
static int remoteSleep(sqlite3_vfs *NotUsed, int microseconds) {
    char argInChar[sizeof(ArgInSleep)];
    memset(argInChar, 0, sizeof(ArgInSleep));
    int rc = SQLITE_OK;

    unixSleepConvertArgInToChar(NotUsed, microseconds, argInChar);
    const char *argOutChar = clientSleep(argInChar, sizeof(ArgInSleep), sizeof(ReturnSleep));
    unixSleepConvertCharToReturn(argOutChar, &rc);

    return rc;
}
//////////////// funtion unixCurrentTime ///////////////////////////////////////////
static int remoteCurrentTime(sqlite3_vfs *NotUsed, double *prNow) {
    char argInChar[sizeof(ArgInCurrentTime)];
    memset(argInChar, 0, sizeof(ArgInCurrentTime));
    int rc = SQLITE_OK;

    unixCurrentTimeConvertArgInToChar(NotUsed, prNow, argInChar);
    const char *argOutChar = clientCurrentTime(argInChar, sizeof(ArgInCurrentTime), sizeof(ReturnCurrentTime));
    unixCurrentTimeConvertCharToReturn(argOutChar, prNow, &rc);

    return rc;
}

//////////////// funtion unixCurrentTimeInt64 ///////////////////////////////////////////
static int remoteCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    char argInChar[sizeof(ArgInCurrentTimeInt64)];
    memset(argInChar, 0, sizeof(ArgInCurrentTimeInt64));
    int rc = SQLITE_OK;

    unixCurrentTimeInt64ConvertArgInToChar(NotUsed, piNow, argInChar);
    const char *argOutChar = clientCurrentTimeInt64(argInChar, sizeof(ArgInCurrentTimeInt64),
                                                    sizeof(struct ReturnCurrentTimeInt64));
    unixCurrentTimeInt64ConvertCharToReturn(argOutChar, piNow, &rc);
    return rc;
}
*/


//////////////// funtion unixGetLastError ///////////////////////////////////////////
static int remoteGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteGetLastError:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInGetLastError)];
    memset(argInChar, 0, sizeof(ArgInGetLastError));
    char argOutChar[sizeof(ReturnGetLastError)];
    memset(argOutChar, 0, sizeof(ReturnGetLastError));
    int rc_errno = 0;

    unixGetLastErrorConvertArgInToChar(NotUsed, NotUsed2, NotUsed3, argInChar);
    clientGetLastError(argInChar, sizeof(ArgInGetLastError), sizeof(ReturnGetLastError), argOutChar);
    unixGetLastErrorConvertCharToReturn(argOutChar, &rc_errno);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteGetLastError: rc_errno=%d\n", rc_errno), debugStr);
#endif
    return rc_errno;
}

//////////////// funtion unixWrite ///////////////////////////////////////////
static int remoteWrite(
        sqlite3_file *id,
        const void *pBuf,
        int amt,
        sqlite3_int64
        offset) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteWrite:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInWrite)];
    memset(argInChar, 0, sizeof(ArgInWrite));
    char argOutChar[sizeof(ReturnWrite)];
    memset(argOutChar, 0, sizeof(ReturnWrite));
    int rc = SQLITE_OK;

    unixWriteConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    clientWrite(argInChar, sizeof(ArgInWrite), sizeof(ReturnWrite), argOutChar);
    unixWriteConvertCharToReturn(argOutChar, id, (char *) pBuf, &amt, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteWrite       : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteRead(sqlite3_file *id, void *pBuf, int amt, sqlite3_int64 offset
) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteRead:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInRead)];
    memset(argInChar, 0, sizeof(ArgInRead));
    char argOutChar[sizeof(ReturnRead)];
    memset(argOutChar, 0, sizeof(ReturnRead));
    int rc = SQLITE_OK;

    unixReadConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    clientRead(argInChar, sizeof(ArgInRead), sizeof(ReturnRead), argOutChar);
    unixReadConvertCharToReturn(argOutChar, id, (char *) pBuf, &amt, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteRead        : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteTruncate(sqlite3_file *id, i64 nByte) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteTruncate:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInTruncate)];
    memset(argInChar, 0, sizeof(ArgInTruncate));
    char argOutChar[sizeof(ReturnTruncate)];
    memset(argOutChar, 0, sizeof(ReturnTruncate));
    int rc = SQLITE_OK;

    unixTruncateConvertArgInToChar(id, nByte, argInChar);
    clientTruncate(argInChar, sizeof(ArgInTruncate), sizeof(ReturnTruncate), argOutChar);
    unixTruncateConvertCharToReturn(argOutChar, id, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteTruncate    : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteSync(sqlite3_file *id, int flags) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteSync:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInSync)];
    memset(argInChar, 0, sizeof(ArgInSync));
    char argOutChar[sizeof(ReturnSync)];
    memset(argOutChar, 0, sizeof(ReturnSync));
    int rc = SQLITE_OK;

    unixSyncConvertArgInToChar(id, flags, argInChar);
    clientSync(argInChar, sizeof(ArgInSync), sizeof(ReturnSync), argOutChar);
    unixSyncConvertCharToReturn(argOutChar, id, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteSync        : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteFileSize(sqlite3_file *id, i64 *pSize) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteFileSize:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInFileSize)];
    memset(argInChar, 0, sizeof(ArgInFileSize));
    char argOutChar[sizeof(ReturnFileSize)];
    memset(argOutChar, 0, sizeof(ReturnFileSize));
    int rc = SQLITE_OK;

    unixFileSizeConvertArgInToChar(id, pSize, argInChar);
    clientFileSize(argInChar, sizeof(ArgInFileSize), sizeof(ReturnFileSize), argOutChar);
    unixFileSizeConvertCharToReturn(argOutChar, id, pSize, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended remoteFileSize    : fd=%d, size=%d, rc=%d\n", ((unixFile *) id)->h, *pSize, rc),
            debugStr);
#endif
    return rc;
}

static int remoteFileControl(sqlite3_file *id, int op, void *pArg) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteFileControl :\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInFileControl)];
    memset(argInChar, 0, sizeof(ArgInFileControl));
    char argOutChar[sizeof(ReturnFileControl)];
    memset(argOutChar, 0, sizeof(ReturnFileControl));
    int rc = SQLITE_OK;
    int size = LenFileControlPArg(op, pArg);

    unixFileControlConvertArgInToChar(id, op, pArg, argInChar, &size);
    clientFileControl(argInChar, sizeof(ArgInFileControl), sizeof(ReturnFileControl), argOutChar);
    unixFileControlConvertCharToReturn(argOutChar, id, pArg, &rc, op);

#if CLIENT_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended remoteFileControl : fd=%d, op=%d, rc=%d\n", ((unixFile *) id)->h, op,
                    rc),
            debugStr);
#endif
//    DebugClient(sprintf(debugStr, "pArg:"), debugStr);
//    DebugChars((char *) pArg, size);
    return rc;
}

static int remoteSectorSize(sqlite3_file *id) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteFileControl:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInSectorSize)];
    memset(argInChar, 0, sizeof(ArgInSectorSize));
    char argOutChar[sizeof(ReturnSectorSize)];
    memset(argOutChar, 0, sizeof(ReturnSectorSize));
    int sectorSize = 0;

    unixSectorSizeConvertArgInToChar(id, argInChar);
    clientSectorSize(argInChar, sizeof(ArgInSectorSize), sizeof(ReturnSectorSize), argOutChar);
    unixSectorSizeConvertCharToReturn(argOutChar, id, &sectorSize);

#if CLIENT_DEBUG_FLAG
    DebugClient(
            sprintf(debugStr, "---ended remoteSectorSize  : fd=%d, sectorSize=%d\n", ((unixFile *) id)->h, sectorSize),
            debugStr);
#endif
    return sectorSize;
}

static int remoteDeviceCharacteristics(sqlite3_file *id) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteDeviceCharacteristics:\n"), debugStr);
#endif


    char argInChar[sizeof(ArgInDeviceCharacteristics)];
    memset(argInChar, 0, sizeof(ArgInDeviceCharacteristics));
    char argOutChar[sizeof(ReturnDeviceCharacteristics)];
    memset(argOutChar, 0, sizeof(ReturnDeviceCharacteristics));
    int deviceCharacteristics = 0;

    unixDeviceCharacteristicsConvertArgInToChar(id, argInChar);
    clientDeviceCharacteristics(argInChar, sizeof(ArgInDeviceCharacteristics),
                                sizeof(ReturnDeviceCharacteristics), argOutChar);
    unixDeviceCharacteristicsConvertCharToReturn(argOutChar, id, &deviceCharacteristics);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteDeviceCharacteristics : %d\n", deviceCharacteristics), debugStr);
#endif
    return deviceCharacteristics;
}

static int remoteClose(sqlite3_file *id) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteClose:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInClose)];
    memset(argInChar, 0, sizeof(ArgInClose));
    char argOutChar[sizeof(ReturnClose)];
    memset(argOutChar, 0, sizeof(ReturnClose));
    int rc = SQLITE_OK;

    unixCloseConvertArgInToChar(id, argInChar);
    clientClose(argInChar, sizeof(ArgInClose), sizeof(ReturnClose), argOutChar);
    unixCloseConvertCharToReturn(argOutChar, id, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteClose       : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteLock(sqlite3_file *id, int eFileLock) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteLock:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInLock)];
    memset(argInChar, 0, sizeof(ArgInLock));
    char argOutChar[sizeof(ReturnLock)];
    memset(argOutChar, 0, sizeof(ReturnLock));
    int rc = SQLITE_OK;

    unixLockConvertArgInToChar(id, eFileLock, argInChar);
    clientLock(argInChar, sizeof(ArgInLock), sizeof(ReturnLock), argOutChar);
    unixLockConvertCharToReturn(argOutChar, id, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteLock        : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteUnlock(sqlite3_file *id, int eFileLock) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteUnlock:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInUnlock)];
    memset(argInChar, 0, sizeof(ArgInUnlock));
    char argOutChar[sizeof(ReturnUnlock)];
    memset(argOutChar, 0, sizeof(ReturnUnlock));
    int rc = SQLITE_OK;

    unixUnlockConvertArgInToChar(id, eFileLock, argInChar);
    clientUnlock(argInChar, sizeof(ArgInUnlock), sizeof(ReturnUnlock), argOutChar);
    unixUnlockConvertCharToReturn(argOutChar, id, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteUnlock      : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteCheckReservedLock(sqlite3_file *id, int *pResOut) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteCheckReservedLock:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInCheckReservedLock)];
    memset(argInChar, 0, sizeof(ArgInCheckReservedLock));
    char argOutChar[sizeof(ReturnCheckReservedLock)];
    memset(argOutChar, 0, sizeof(ReturnCheckReservedLock));
    int rc = SQLITE_OK;

    unixCheckReservedLockConvertArgInToChar(id, pResOut, argInChar);
    clientCheckReservedLock(argInChar, sizeof(ArgInCheckReservedLock),
                            sizeof(ReturnCheckReservedLock), argOutChar);
    unixCheckReservedLockConvertCharToReturn(argOutChar, id, pResOut, &rc);
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteCheckReservedLock : fd=%d, rc=%d\n", ((unixFile *) id)->h, rc),
                debugStr);
#endif
    return rc;
}

static int remoteFetch(sqlite3_file *fd, i64 iOff, int nAmt, void **pp) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteFetch:\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInFetch)];
    memset(argInChar, 0, sizeof(ArgInFetch));
    char argOutChar[sizeof(ReturnFetch)];
    memset(argOutChar, 0, sizeof(ReturnFetch));
    int rc = SQLITE_OK;

    unixFetchConvertArgInToChar(fd, iOff, nAmt, pp, argInChar);
    clientFetch(argInChar, sizeof(ArgInFetch), sizeof(struct ReturnFetch), argOutChar);
    char str[nAmt];
    unixFetchConvertCharToReturn(argOutChar, fd, str, nAmt, &rc);
    *pp = str;

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteFetch       : fd=%d, rc=%d\n", ((unixFile *) fd)->h, rc), debugStr);
#endif
    return rc;
}

static int remoteUnfetch(sqlite3_file *fd, i64 iOff, void *p) {
#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---start remoteUnfetch.\n"), debugStr);
#endif

    char argInChar[sizeof(ArgInUnfetch)];
    memset(argInChar, 0, sizeof(ArgInUnfetch));
    char argOutChar[sizeof(ReturnUnfetch)];
    memset(argOutChar, 0, sizeof(ReturnUnfetch));
    int rc = SQLITE_OK;

    int p_flag;
    p_flag = (p == 0 ? 0 : 1);
#if CLIENT_DEBUG_FLAG
    printf("p_flag = %d\n", p_flag);
#endif
    unixUnfetchConvertArgInToChar(fd, iOff, &p_flag, argInChar);
    clientUnfetch(argInChar, sizeof(ArgInUnfetch), sizeof(ReturnUnfetch), argOutChar);
    unixUnfetchConvertCharToReturn(argOutChar, fd, &rc);

#if CLIENT_DEBUG_FLAG
    DebugClient(sprintf(debugStr, "---ended remoteUnfetch     : fd=%d, rc=%d\n", ((unixFile *) fd)->h, rc), debugStr);
#endif
    return rc;
}
//static int remote ( ){
//    char argInChar[sizeof( )];
//    memset(argInChar, 0, sizeof());
//    int rc = SQLITE_OK;ll
//
//    unix
//    const char* argOutChar = client (argInChar, sizeof( ), sizeof( ));
//    unix
//
//    return rc;
//}



// /*
//  * 测试函数
//  */

// void Test_Init() {
//     int rc = remote_sqlite3_os_init();

//     printf("%s\n", "---Test_Init:");
//     printf("   rc = %d\n", rc);
// }

// void Test_Open() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000004 & 0x87f7f, &outFlag);

//     printf("%s\n", "---remoteOpen:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_Delete() {
//     int rc = remoteDelete(&aVfs[0], "./remote.db", 1);

//     printf("%s\n", "---remoteDelete:");
//     printf("   rc = %d\n", rc);
// }

// void Test_Access() {
//     int res;
//     int rc = remoteAccess(&aVfs[0], "./remote.db", 0, &res);

//     printf("%s\n", "---remoteAccess:");
//     printf("   pResOut = %d\n", res);
//     printf("   rc = %d\n", rc);
// }

// void Test_FullPathname() {
//     char zOut[512];
//     int rc = remoteFullPathname(&aVfs[0], "remote.db", 1, zOut);

//     printf("%s\n", "---remoteFullPathname:");
//     printf("   zOut:%s\n", zOut);
//     printf("   rc = %d\n", rc);    // TODO: rc = 14, 可能传的参数 nOut不对
// }

// void Test_GetLastError() {
//     int rc_errno = remoteGetLastError(&aVfs[0], 0, "0");
//     printf("%s\n", "---remoteGetLastError:");
//     printf("   rc_errno = %d\n", rc_errno);   // TODO: 返回 errno = 11
// }

// void Test_Write() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     printf("%s\n", "---remoteWrite:");
//     printf("%s\n", "   open bofore Write:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);

//     fi;
//     char *pBuf = "adsfgdskajfdlajsldkjl";
//     rc = remoteWrite((sqlite3_file *) &fi, (void *) pBuf, 10, 0);

//     printf("%s\n", "---Write:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);   // TODO: 可以写进去，但是一直返回 rc = 4 或 10
// }

// void Test_Read() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     printf("%s\n", "---remoteRead:");
//     printf("%s\n", "   open bofore Read:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);

//     char pBuf[4];
//    memset(argInChar, 0, sizeof());
//     rc = remoteRead((sqlite3_file *) &fi, pBuf, 4, 2);

//     printf("%s\n", "---Read:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   pBuf = %s\n", pBuf);
//     printf("   rc = %d\n", rc);
// }

// void Test_Truncate() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     rc = remoteTruncate((sqlite3_file *) &fi, 10);

//     printf("%s\n", "---remoteTruncate:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_Sync() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     rc = remoteSync((sqlite3_file *) &fi, 0);

//     printf("%s\n", "---remoteSync:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_FileSize() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     i64 pSize;
//     rc = remoteFileSize((sqlite3_file *) &fi, &pSize);

//     printf("%s\n", "---remoteSync:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   pSize = %d\n", pSize);
//     printf("   rc = %d\n", rc);
// }

// void Test_FileControl() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     int pArg = 0;
//     rc = remoteFileControl((sqlite3_file *) &fi, SQLITE_FCNTL_SYNC, &pArg);

//     printf("%s\n", "---remoteFileControl:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   pArg = %d\n", pArg);
//     printf("   rc = %d\n", rc);
// }

// void Test_SectorSize() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     int sectorSize = remoteSectorSize((sqlite3_file *) &fi);
//     printf("%s\n", "---remoteSectorSize:");
//     printf("   sectorSize = %d\n", sectorSize);
// }

// void Test_DeviceCharacteristics() {
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

//     int deviceCharacteristics = remoteDeviceCharacteristics((sqlite3_file *) &fi);
//     printf("%s\n", "---remoteDeviceCharacteristics:");
//     printf("   deviceCharacteristics = %d\n", deviceCharacteristics);
// }

// void Test_Close() {
//     int rc = remote_sqlite3_os_init();

//     unixFile fi;
//     int outFlag = 2;
//     rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, SQLITE_OPEN_CREATE & 0x87f7f, &outFlag);
//     printf("   outFlag = %d\n", outFlag);

//     rc = remoteClose((sqlite3_file *) &fi);

//     printf("%s\n", "---remoteClose:");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_Lock_And_Unlock() {
//     printf("---Lock_And_Unlock:\n");

//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);
//     printf("    opened : fi.h = %d\n", "fi.h");

//     rc = remoteLock((sqlite3_file *) &fi, EXCLUSIVE_LOCK);
//     printf("   lock:\n");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);

//     rc = remoteUnlock((sqlite3_file *) &fi, NO_LOCK);
//     printf("   unlock:\n");
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_CheckReservedLock() {
//     printf("---remoteCheckReservedLock:\n");
//     unixFile fi;
//     int outFlag = 2;
//     int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000004 & 0x87f7f, &outFlag);
//     printf("    opened : fi.h = %d\n", fi.h);
// //    printf("    fi.h.Inode.nRef = %d\n", fi.pInode->nRef);

//     int locked = 0;
// //    rc = remoteCheckReservedLock((sqlite3_file *) &fi, &locked);

//     printf("   locked:%d\n", locked);
//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// void Test_Fetch() {
//     printf("---remoteFetch:\n");
//     unixFile fi;
//     fi.h = 9;
//     i64 ioff = 10;
//     int nAmt = 2;
//     void *pData;

//     int rc = remoteFetch((sqlite3_file *) &fi, ioff, nAmt, &pData);

//     printf("   f.h = %d\n", fi.h);
//     printf("   ioff = %ld\n", ioff);
//     printf("   nAmt = %d\n", nAmt);
//     printf("   pp = %s\n", (char *) pData);
//     printf("   rc = %d\n", rc);
// }

// void Test_Unfetch() {
//     printf("---remoteUnfetch:\n");
//     unixFile fi;
//     fi.h = 9;
//     void *pData = &fi;

//     int rc = remoteUnfetch((sqlite3_file *) &fi, 0, pData);

//     printf("   f.h = %d\n", fi.h);
//     printf("   rc = %d\n", rc);
// }

// int main() {
// //    Test_Init();
// //    Test_Open();
// //    Test_Delete();
// //    Test_Access();
// //    Test_FullPathname();
// //    Test_Randomness();
// //    Test_Sleep();
// //    Test_CurrentTime();
// //    Test_CurrentTimeInt64();
// //    Test_GetLastError();
// //    Test_Write();
// //    Test_Read();
// //    Test_Truncate();
// //    Test_Sync();
// //    Test_FileSize();
// //    Test_FileControl();
// //    Test_SectorSize();
// //    Test_DeviceCharacteristics();
// //    Test_Close();
// //    Test_Lock_And_Unlock();
// //    Test_CheckReservedLock();
// //    Test_Fetch();

// //    Test_Unfetch();
//     return 0;
// }
