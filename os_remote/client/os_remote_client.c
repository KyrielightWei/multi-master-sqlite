#include "../common/os_arg_convert.c"

extern const char *clientInit();
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


//////////////// funtion unixOpen ///////////////////////////////////////////
static int remoteOpen(
        sqlite3_vfs *pVfs,           /* The VFS for which this is the xOpen method */
        const char *zPath,           /* Pathname of file to be opened */
        sqlite3_file *pFile,         /* The file descriptor to be filled in */
        int flags,                   /* Input flags to control the opening */
        int *pOutFlags               /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixOpen)] = {'\0'};
    int rc = SQLITE_OK;

    unixOpenConvertArgInToChar(pVfs, zPath, pFile, flags, pOutFlags, argInChar);
    const char *argOutChar = clientOpen(argInChar, sizeof(ArgInUnixOpen), sizeof(ReturnUnixOpen));
    unixOpenConvertCharToReturn(argOutChar, pFile, pOutFlags, &rc);

    return rc;
}
//////////////// funtion unixDelete ///////////////////////////////////////////
static int remoteDelete(
        sqlite3_vfs *NotUsed,     /* VFS containing this as the xDelete method */
        const char *zPath,        /* Name of file to be deleted */
        int dirSync             /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixDelete)] = {'\0'};
    int rc = SQLITE_OK;

    unixDeleteConvertArgInToChar(NotUsed, zPath, dirSync, argInChar);
    const char *argOutChar = clientDelete(argInChar, sizeof(ArgInUnixDelete), sizeof(ReturnUnixDelete));
    unixDeleteConvertCharToReturn(argOutChar, &rc);

    return rc;
}
//////////////// funtion unixAccess ///////////////////////////////////////////
static int remoteAccess(
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut            /* Write result boolean here */
) {
    char argInChar[sizeof(ArgInUnixAccess)] = {'\0'};
    int rc = SQLITE_OK;

    unixAccessConvertArgInToChar(NotUsed, zPath, flags, pResOut, argInChar);
    const char *argOutChar = clientAccess(argInChar, sizeof(ArgInUnixAccess), sizeof(ReturnUnixAccess));
    unixAccessConvertCharToReturn(argOutChar, pResOut, &rc);

    return rc;
}
//////////////// funtion unixFullPathname ///////////////////////////////////////////
static int remoteFullPathname(
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        const char *zPath,            /* Possibly relative input path */
        int nOut,                     /* Size of output buffer in bytes */
        char *zOut
) {
    char argInChar[sizeof(ArgInUnixFullPathname)] = {'\0'};
    int rc = SQLITE_OK;

    unixFullPathnameConvertArgInToChar(pVfs, zPath, nOut, zOut, argInChar);
    const char *argOutChar = clientFullPathname(argInChar, sizeof(ArgInUnixFullPathname),
                                                sizeof(ReturnUnixFullPathname));
    unixFullPathnameConvertCharToReturn(argOutChar, zOut, &rc);

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
    char argInChar[sizeof(ArgInUnixRandomness)] = {'\0'};
    int rc = SQLITE_OK;

    unixRandomnessConvertArgInToChar(NotUsed, nBuf, zBuf, argInChar);
    const char *argOutChar = clientRandomness(argInChar, sizeof(ArgInUnixRandomness), sizeof(ReturnUnixRandomness));
    unixRandomnessConvertCharToReturn(argOutChar, zBuf, &rc);
    return rc;
}
//////////////// funtion unixSleep ///////////////////////////////////////////
static int remoteSleep(sqlite3_vfs *NotUsed, int microseconds) {
    char argInChar[sizeof(ArgInUnixSleep)] = {'\0'};
    int rc = SQLITE_OK;

    unixSleepConvertArgInToChar(NotUsed, microseconds, argInChar);
    const char *argOutChar = clientSleep(argInChar, sizeof(ArgInUnixSleep), sizeof(ReturnUnixSleep));
    unixSleepConvertCharToReturn(argOutChar, &rc);

    return rc;
}
//////////////// funtion unixCurrentTime ///////////////////////////////////////////
static int remoteCurrentTime(sqlite3_vfs *NotUsed, double *prNow) {
    char argInChar[sizeof(ArgInUnixCurrentTime)] = {'\0'};
    int rc = SQLITE_OK;

    unixCurrentTimeConvertArgInToChar(NotUsed, prNow, argInChar);
    const char *argOutChar = clientCurrentTime(argInChar, sizeof(ArgInUnixCurrentTime), sizeof(ReturnUnixCurrentTime));
    unixCurrentTimeConvertCharToReturn(argOutChar, prNow, &rc);

    return rc;
}

//////////////// funtion unixCurrentTimeInt64 ///////////////////////////////////////////
static int remoteCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    char argInChar[sizeof(ArgInUnixCurrentTimeInt64)] = {'\0'};
    int rc = SQLITE_OK;

    unixCurrentTimeInt64ConvertArgInToChar(NotUsed, piNow, argInChar);
    const char *argOutChar = clientCurrentTimeInt64(argInChar, sizeof(ArgInUnixCurrentTimeInt64),
                                                    sizeof(struct ReturnUnixCurrentTimeInt64));
    unixCurrentTimeInt64ConvertCharToReturn(argOutChar, piNow, &rc);
    return rc;
}
*/


//////////////// funtion unixGetLastError ///////////////////////////////////////////
static int remoteGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3) {
    char argInChar[sizeof(ArgInUnixGetLastError)] = {'\0'};
    int rc_errno = 0;

    unixGetLastErrorConvertArgInToChar(NotUsed, NotUsed2, NotUsed3, argInChar);
    const char *argOutChar = clientGetLastError(argInChar, sizeof(ArgInUnixGetLastError),
                                                sizeof(ReturnUnixGetLastError));
    unixGetLastErrorConvertCharToReturn(argOutChar, &rc_errno);
    return rc_errno;
}

//////////////// funtion unixWrite ///////////////////////////////////////////
static int remoteWrite(
        sqlite3_file *id,
        const void *pBuf,
        int amt,
        sqlite3_int64 offset) {
    char argInChar[sizeof(ArgInWrite)] = {'\0'};
    int rc = SQLITE_OK;

//    char *tempBuf = (char *) pBuf;
    unixWriteConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    const char *argOutChar = clientWrite(argInChar, sizeof(ArgInWrite), sizeof(ReturnWrite));
    unixWriteConvertCharToReturn(argInChar, id, (char *) pBuf, &amt, &rc);
//    pBuf = tempBuf;

    return rc;
}
static int remoteRead(
        sqlite3_file *id,
        void *pBuf,
        int amt,
        sqlite3_int64 offset
) {
    char argInChar[sizeof(ArgInUnixRead)] = {'\0'};
    int rc = SQLITE_OK;

    unixReadConvertArgInToChar(id, (char *) pBuf, amt, offset, argInChar);
    printf("   id.h = %d\n", ((unixFile *) id)->h);
    const char *argOutChar = clientRead(argInChar, sizeof(ArgInUnixRead), sizeof(ReturnUnixRead));
    unixReadConvertCharToReturn(argOutChar, id, (char *) pBuf, &amt, &rc);
    printf("   id.h = %d\n", ((unixFile *) id)->h);
    return rc;
}





/*
 * 测试函数
 */
void Test_Open() {
    unixFile fi;
    int outFlag = 2;
    int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000004 & 0x87f7f, &outFlag);

    printf("%s\n", "---remoteOpen:");
    printf("   f.h = %d\n", fi.h);
    printf("   rc = %d\n", rc);
}
void Test_Delete() {
    int rc = remoteDelete(&aVfs[0], "./remote.db", 1);

    printf("%s\n", "---remoteDelete:");
    printf("   rc = %d\n", rc);
}
void Test_Access() {
    int res;
    int rc = remoteAccess(&aVfs[0], "./remote.db", 0, &res);

    printf("%s\n", "---remoteAccess:");
    printf("   pResOut = %d\n", res);
    printf("   rc = %d\n", rc);
}
void Test_FullPathname() {
    char zOut[512];
    int rc = remoteFullPathname(&aVfs[0], "remote.db", 1, zOut);

    printf("%s\n", "---remoteFullPathname:");
    printf("   zOut:%s\n", zOut);
    printf("   rc = %d\n", rc);    // TODO: rc = 14, 可能传的参数 nOut不对
}
/*
void Test_Randomness() {
    char zBuf[256];
    int rc = remoteRandomness(&aVfs[0], 256, zBuf);

    printf("%s\n", "---remoteRandomness:");
    printf("   zBuf:%s\n", zBuf);
    printf("   rc = %d\n", rc);
}

void Test_Sleep() {
    int rc = remoteSleep(&aVfs[0], 1000);

    printf("%s\n", "---remoteSleep:");
    printf("rc = %d\n", rc);
}
void Test_CurrentTime() {
    double prNow;
    int rc = remoteCurrentTime(&aVfs[0], &prNow);

    printf("%s\n", "---remoteCurrentTime:");
    printf("   prNow:%f\n", prNow);
    printf("   rc = %d\n", rc);

}
void Test_CurrentTimeInt64() {
    sqlite3_int64 piNow;
    int rc = remoteCurrentTimeInt64(&aVfs[0], &piNow);

    printf("%s\n", "---remoteCurrentTimeInt64:");
    printf("   piNow = %d\n", piNow);
    printf("   rc = %d\n", rc);
}
 */
void Test_GetLastError() {
    int rc_errno = remoteGetLastError(&aVfs[0], 0, "0");
    printf("%s\n", "---remoteGetLastError:");
    printf("   rc_errno = %d\n", rc_errno);   // TODO: 返回 errno = 11
}
void Test_Write() {
    unixFile fi;
    int outFlag = 2;
    int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

    printf("%s\n", "---Write:");
    printf("%s\n", "   open bofore Write:");
    printf("   f.h = %d\n", fi.h);
    printf("   rc = %d\n", rc);

    fi;
    char *pBuf = "adsfgdskajfdlajsldkjl";
    rc = remoteWrite((sqlite3_file *) &fi, (void*)pBuf, 10, 0);

    printf("%s\n", "---Write:");
    printf("   f.h = %d\n", fi.h);
    printf("   rc = %d\n", rc);   // TODO: 可以写进去，但是一直返回 rc = 4 或 10
}
void Test_Read() {
    unixFile fi;
    int outFlag = 2;
    int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

    printf("%s\n", "---Read:");
    printf("%s\n", "   open bofore Read:");
    printf("   f.h = %d\n", fi.h);
    printf("   rc = %d\n", rc);

    char pBuf[4] = {'\0'};
    rc = remoteRead((sqlite3_file *) &fi, pBuf, 4, 2);

    printf("%s\n", "---Read:");
    printf("   f.h = %d\n", fi.h);
    printf("   pBuf = %s\n", pBuf);
    printf("   rc = %d\n", rc);
}


int main() {
//    Test_Init();
//    Test_Open();
//    Test_Delete();
//    Test_Access();
//    Test_FullPathname();
//    Test_Randomness();
//    Test_Sleep();
//    Test_CurrentTime();
//    Test_CurrentTimeInt64();
//    Test_GetLastError();
    Test_Write();
    Test_Read();

    return 0;
}
