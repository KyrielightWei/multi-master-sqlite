#include "../common/os_arg_convert.c"
#include "util.c"

extern const char *clientInit(char *argin, u32 inlen, u32 outlen);

extern const char *clientOpen(char *argin, u32 inlen, u32 outlen);

extern const char *clientDelete(char *argin, u32 inlen, u32 outlen);

extern const char *clientAccess(char *argin, u32 inlen, u32 outlen);

extern const char *clientFullPathname(char *argin, u32 inlen, u32 outlen);
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
extern const char *clientGetLastError(char *argin, u32 inlen, u32 outlen);

extern const char *clientWrite(char *argin, u32 inlen, u32 outlen);

extern const char *clientRead(char *argin, u32 inlen, u32 outlen);

extern const char *clientTruncate(char *argin, u32 inlen, u32 outlen);

extern const char *clientSync(char *argin, u32 inlen, u32 outlen);

extern const char *clientFileSize(char *argin, u32 inlen, u32 outlen);

extern const char *clientFileControl(char *argin, u32 inlen, u32 outlen);

extern const char *clientSectorSize(char *argin, u32 inlen, u32 outlen);

extern const char *clientDeviceCharacteristics(char *argin, u32 inlen, u32 outlen);

extern const char *clientClose(char *argin, u32 inlen, u32 outlen);

extern const char *clientLock(char *argin, u32 inlen, u32 outlen);

extern const char *clientUnlock(char *argin, u32 inlen, u32 outlen);

extern const char *clientCheckReservedLock(char *argin, u32 inlen, u32 outlen);

extern const char *clientFetch(char *argin, u32 inlen, u32 outlen);

extern const char *clientUnfetch(char *argin, u32 inlen, u32 outlen);


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

//////////////// funtion unixOpen ///////////////////////////////////////////
int remote_sqlite3_os_init(void) {
    DebugClient(sprintf(debugStr, "---start remote_sqlite3_os_init:\n"), debugStr);
    char argInChar[sizeof(ArgInInit)];
    int rc = SQLITE_OK;
    const char *argOutChar = clientInit(argInChar, sizeof(ArgInInit), sizeof(struct ReturnInit));
    sqlite3_os_initConvertCharToReturn(argOutChar, &rc);
    DebugClient(sprintf(debugStr, "---end remote_sqlite3_os_init:\n"), debugStr);
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
    DebugClient(sprintf(debugStr, "---start remoteOpen:\n"), debugStr);
    char argInChar[sizeof(ArgInOpen)] = {'\0'};
    int rc = SQLITE_OK;

    unixOpenConvertArgInToChar(pVfs, zPath, pFile, flags, pOutFlags, argInChar);
    const char *argOutChar = clientOpen(argInChar, sizeof(ArgInOpen), sizeof(ReturnOpen));
    unixOpenConvertCharToReturn(argOutChar, pFile, pOutFlags, &rc);

    DebugClient(sprintf(debugStr, "---end remoteOpen:\n"), debugStr);
    return rc;
}

//////////////// funtion unixDelete ///////////////////////////////////////////
static int remoteDelete(
        sqlite3_vfs *NotUsed,     /* VFS containing this as the xDelete method */
        const char *zPath,        /* Name of file to be deleted */
        int dirSync             /* Output flags returned to SQLite core */
) {
    DebugClient(sprintf(debugStr, "---start remoteDelete:\n"), debugStr);
    char argInChar[sizeof(ArgInDelete)] = {'\0'};
    int rc = SQLITE_OK;

    unixDeleteConvertArgInToChar(NotUsed, zPath, dirSync, argInChar);
    const char *argOutChar = clientDelete(argInChar, sizeof(ArgInDelete), sizeof(ReturnDelete));
    unixDeleteConvertCharToReturn(argOutChar, &rc);

    DebugClient(sprintf(debugStr, "---end remoteDelete:\n"), debugStr);
    return rc;
}

//////////////// funtion unixAccess ///////////////////////////////////////////
static int remoteAccess(
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut            /* Write result boolean here */
) {
    DebugClient(sprintf(debugStr, "---start remoteAccess:\n"), debugStr);
    char argInChar[sizeof(ArgInAccess)] = {'\0'};
    int rc = SQLITE_OK;

    unixAccessConvertArgInToChar(NotUsed, zPath, flags, pResOut, argInChar);
    const char *argOutChar = clientAccess(argInChar, sizeof(ArgInAccess), sizeof(ReturnAccess));
    unixAccessConvertCharToReturn(argOutChar, pResOut, &rc);
    DebugClient(sprintf(debugStr, "---end remoteAccess:\n"), debugStr);
    return rc;
}

//////////////// funtion unixFullPathname ///////////////////////////////////////////
static int remoteFullPathname(
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        const char *zPath,            /* Possibly relative input path */
        int nOut,                     /* Size of output buffer in bytes */
        char *zOut
) {
    DebugClient(sprintf(debugStr, "---start remoteFullPathname:\n"), debugStr);
    char argInChar[sizeof(ArgInFullPathname)] = {'\0'};
    int rc = SQLITE_OK;

    unixFullPathnameConvertArgInToChar(pVfs, zPath, nOut, zOut, argInChar);
    const char *argOutChar = clientFullPathname(argInChar, sizeof(ArgInFullPathname),
                                                sizeof(ReturnFullPathname));
    unixFullPathnameConvertCharToReturn(argOutChar, zOut, &rc);
    DebugClient(sprintf(debugStr, "---end remoteFullPathname:\n"), debugStr);
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
    char argInChar[sizeof(ArgInRandomness)] = {'\0'};
    int rc = SQLITE_OK;

    unixRandomnessConvertArgInToChar(NotUsed, nBuf, zBuf, argInChar);
    const char *argOutChar = clientRandomness(argInChar, sizeof(ArgInRandomness), sizeof(ReturnRandomness));
    unixRandomnessConvertCharToReturn(argOutChar, zBuf, &rc);
    return rc;
}
//////////////// funtion unixSleep ///////////////////////////////////////////
static int remoteSleep(sqlite3_vfs *NotUsed, int microseconds) {
    char argInChar[sizeof(ArgInSleep)] = {'\0'};
    int rc = SQLITE_OK;

    unixSleepConvertArgInToChar(NotUsed, microseconds, argInChar);
    const char *argOutChar = clientSleep(argInChar, sizeof(ArgInSleep), sizeof(ReturnSleep));
    unixSleepConvertCharToReturn(argOutChar, &rc);

    return rc;
}
//////////////// funtion unixCurrentTime ///////////////////////////////////////////
static int remoteCurrentTime(sqlite3_vfs *NotUsed, double *prNow) {
    char argInChar[sizeof(ArgInCurrentTime)] = {'\0'};
    int rc = SQLITE_OK;

    unixCurrentTimeConvertArgInToChar(NotUsed, prNow, argInChar);
    const char *argOutChar = clientCurrentTime(argInChar, sizeof(ArgInCurrentTime), sizeof(ReturnCurrentTime));
    unixCurrentTimeConvertCharToReturn(argOutChar, prNow, &rc);

    return rc;
}

//////////////// funtion unixCurrentTimeInt64 ///////////////////////////////////////////
static int remoteCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    char argInChar[sizeof(ArgInCurrentTimeInt64)] = {'\0'};
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
    DebugClient(sprintf(debugStr, "---start remoteGetLastError:\n"), debugStr);
    char argInChar[sizeof(ArgInGetLastError)] = {'\0'};
    int rc_errno = 0;

    unixGetLastErrorConvertArgInToChar(NotUsed, NotUsed2, NotUsed3, argInChar);
    const char *argOutChar = clientGetLastError(argInChar, sizeof(ArgInGetLastError),
                                                sizeof(ReturnGetLastError));
    unixGetLastErrorConvertCharToReturn(argOutChar, &rc_errno);
    DebugClient(sprintf(debugStr, "---end remoteGetLastError:\n"), debugStr);
    return rc_errno;
}

//////////////// funtion unixWrite ///////////////////////////////////////////
static int remoteWrite(
        sqlite3_file *id,
        const void *pBuf,
        int amt,
        sqlite3_int64
        offset) {
    DebugClient(sprintf(debugStr, "---start remoteWrite:\n"), debugStr);
    char argInChar[sizeof(ArgInWrite)] = {'\0'};
    int rc = SQLITE_OK;

    unixWriteConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    const char *argOutChar = clientWrite(argInChar, sizeof(ArgInWrite), sizeof(ReturnWrite));
    unixWriteConvertCharToReturn(argInChar, id, (char *) pBuf, &amt, &rc);

    DebugClient(sprintf(debugStr, "---end remoteWrite:\n"), debugStr);
    return rc;
}

static int remoteRead(sqlite3_file *id, void *pBuf, int amt, sqlite3_int64 offset
) {
    DebugClient(sprintf(debugStr, "---start remoteRead:\n"), debugStr);
    char argInChar[sizeof(ArgInRead)] = {'\0'};
    int rc = SQLITE_OK;

    unixReadConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    printf("   id.h = %d\n", ((unixFile *) id)->h);
    const char *argOutChar = clientRead(argInChar, sizeof(ArgInRead), sizeof(ReturnRead));
    unixReadConvertCharToReturn(argOutChar, id, (char *) pBuf, &amt, &rc);
    printf("   id.h = %d\n", ((unixFile *) id)->h);

    DebugClient(sprintf(debugStr, "---end remoteRead:\n"), debugStr);
    return rc;
}

static int remoteTruncate(sqlite3_file *id, i64 nByte) {
    DebugClient(sprintf(debugStr, "---start remoteTruncate:\n"), debugStr);
    char argInChar[sizeof(ArgInTruncate)] = {'\0'};
    int rc = SQLITE_OK;

    unixTruncateConvertArgInToChar(id, nByte, argInChar);
    const char *argOutChar = clientTruncate(argInChar, sizeof(ArgInTruncate), sizeof(ReturnTruncate));
    unixTruncateConvertCharToReturn(argOutChar, id, &rc);
    DebugClient(sprintf(debugStr, "---end remoteTruncate:\n"), debugStr);
    return rc;
}

static int remoteSync(sqlite3_file *id, int flags) {
    DebugClient(sprintf(debugStr, "---start remoteSync:\n"), debugStr);
    char argInChar[sizeof(ArgInSync)] = {'\0'};
    int rc = SQLITE_OK;

    unixSyncConvertArgInToChar(id, flags, argInChar);
    const char *argOutChar = clientSync(argInChar, sizeof(ArgInSync), sizeof(ReturnSync));
    unixSyncConvertCharToReturn(argOutChar, id, &rc);
    DebugClient(sprintf(debugStr, "---end remoteSync:\n"), debugStr);
    return rc;
}

static int remoteFileSize(sqlite3_file *id, i64 *pSize) {
    DebugClient(sprintf(debugStr, "---start remoteFileSize:\n"), debugStr);
    char argInChar[sizeof(ArgInFileSize)] = {'\0'};
    int rc = SQLITE_OK;

    unixFileSizeConvertArgInToChar(id, pSize, argInChar);
    const char *argOutChar = clientFileSize(argInChar, sizeof(ArgInFileSize), sizeof(ReturnFileSize));
    unixFileSizeConvertCharToReturn(argOutChar, id, pSize, &rc);
    DebugClient(sprintf(debugStr, "---end remoteFileSize:\n"), debugStr);
    return rc;
}

static int remoteFileControl(sqlite3_file *id, int op, void *pArg) {
    DebugClient(sprintf(debugStr, "---start remoteFileControl:\n"), debugStr);
    char argInChar[sizeof(ArgInFileControl)] = {'\0'};
    int rc = SQLITE_OK;

    unixFileControlConvertArgInToChar(id, op, pArg, argInChar);
    const char *argOutChar = clientFileControl(argInChar, sizeof(ArgInFileControl), sizeof(ReturnFileControl));
    unixFileControlConvertCharToReturn(argOutChar, id, pArg, &rc);
    DebugClient(sprintf(debugStr, "---end remoteDelete:\n"), debugStr);
    return rc;
}

static int remoteSectorSize(sqlite3_file *id) {
    DebugClient(sprintf(debugStr, "---start remoteFileControl:\n"), debugStr);
    char argInChar[sizeof(ArgInSectorSize)] = {'\0'};
    int sectorSize = 0;

    unixSectorSizeConvertArgInToChar(id, argInChar);
    const char *argOutChar = clientSectorSize(argInChar, sizeof(ArgInSectorSize), sizeof(ReturnSectorSize));
    unixSectorSizeConvertCharToReturn(argOutChar, id, &sectorSize);
    DebugClient(sprintf(debugStr, "---end remoteSectorSize:\n"), debugStr);
    return sectorSize;
}

static int remoteDeviceCharacteristics(sqlite3_file *id) {
    DebugClient(sprintf(debugStr, "---start remoteDeviceCharacteristics:\n"), debugStr);
    char argInChar[sizeof(ArgInDeviceCharacteristics)] = {'\0'};
    int deviceCharacteristics = 0;

    unixDeviceCharacteristicsConvertArgInToChar(id, argInChar);
    const char *argOutChar = clientDeviceCharacteristics(argInChar, sizeof(ArgInDeviceCharacteristics),
                                                         sizeof(ReturnDeviceCharacteristics));
    unixDeviceCharacteristicsConvertCharToReturn(argOutChar, id, &deviceCharacteristics);
    DebugClient(sprintf(debugStr, "---end remoteDeviceCharacteristics:\n"), debugStr);
    return deviceCharacteristics;
}

static int remoteClose(sqlite3_file *id) {
    DebugClient(sprintf(debugStr, "---start remoteClose:\n"), debugStr);
    char argInChar[sizeof(ArgInClose)] = {'\0'};
    int rc = SQLITE_OK;

    unixCloseConvertArgInToChar(id, argInChar);
    const char *argOutChar = clientClose(argInChar, sizeof(ArgInClose), sizeof(ReturnClose));
    unixCloseConvertCharToReturn(argOutChar, id, &rc);
    DebugClient(sprintf(debugStr, "---end remoteClose:\n"), debugStr);
    return rc;
}

static int remoteLock(sqlite3_file *id, int eFileLock) {
    DebugClient(sprintf(debugStr, "---start remoteLock:\n"), debugStr);
    char argInChar[sizeof(ArgInLock)] = {'\0'};
    int rc = SQLITE_OK;

    unixLockConvertArgInToChar(id, eFileLock, argInChar);
    const char *argOutChar = clientLock(argInChar, sizeof(ArgInLock), sizeof(ReturnLock));
    unixLockConvertCharToReturn(argOutChar, id, &rc);
    DebugClient(sprintf(debugStr, "---end remoteLock:\n"), debugStr);
    return rc;
}

static int remoteUnlock(sqlite3_file *id, int eFileLock) {
    DebugClient(sprintf(debugStr, "---start remoteUnlock:\n"), debugStr);
    char argInChar[sizeof(ArgInUnlock)] = {'\0'};
    int rc = SQLITE_OK;

    unixUnlockConvertArgInToChar(id, eFileLock, argInChar);
    const char *argOutChar = clientUnlock(argInChar, sizeof(ArgInUnlock), sizeof(ReturnUnlock));
    unixUnlockConvertCharToReturn(argOutChar, id, &rc);
    DebugClient(sprintf(debugStr, "---end remoteUnlock:\n"), debugStr);
    return rc;
}

static int remoteCheckReservedLock(sqlite3_file *id, int *pResOut) {
    DebugClient(sprintf(debugStr, "---start remoteCheckReservedLock:\n"), debugStr);
    char argInChar[sizeof(ArgInCheckReservedLock)] = {'\0'};
    int rc = SQLITE_OK;

    unixCheckReservedLockConvertArgInToChar(id, pResOut, argInChar);
    const char *argOutChar = clientCheckReservedLock(argInChar, sizeof(ArgInCheckReservedLock),
                                                     sizeof(ReturnCheckReservedLock));
    unixCheckReservedLockConvertCharToReturn(argOutChar, id, pResOut, &rc);
    DebugClient(sprintf(debugStr, "---end remoteCheckReservedLock:\n"), debugStr);
    return rc;
}

static int remoteFetch(sqlite3_file *fd, i64 iOff, int nAmt, void **pp) {
    DebugClient(sprintf(debugStr, "---start remoteFetch:\n"), debugStr);
    char argInChar[sizeof(ArgInFetch)] = {'\0'};
    int rc = SQLITE_OK;

    unixFetchConvertArgInToChar(fd, iOff, nAmt, pp, argInChar);
    const char *argOutChar = clientFetch(argInChar, sizeof(ArgInFetch), sizeof(struct ReturnFetch));

    char str[nAmt];
    unixFetchConvertCharToReturn(argOutChar, fd, str, nAmt, &rc);
    *pp = str;
    DebugClient(sprintf(debugStr, "---end remoteFetch:\n"), debugStr);
    return rc;
}

static int remoteUnfetch(sqlite3_file *fd, i64 iOff, void *p) {
    char argInChar[sizeof(ArgInUnfetch)] = {'\0'};
    int rc = SQLITE_OK;

    int p_flag; p_flag = (p == 0 ?  0 : 1);
    printf("p_flag = %d\n", p_flag);
    unixUnfetchConvertArgInToChar(fd, iOff, &p_flag, argInChar);
    const char *argOutChar = clientUnfetch(argInChar, sizeof(ArgInUnfetch), sizeof(ReturnUnfetch));

    unixUnfetchConvertCharToReturn(argOutChar, fd, &rc);

    return rc;
}
//static int remote ( ){
//    char argInChar[sizeof( )] = {'\0'};
//    int rc = SQLITE_OK;
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

//     char pBuf[4] = {'\0'};
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
