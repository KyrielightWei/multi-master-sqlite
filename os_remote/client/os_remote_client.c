#include "os_arg_convert.c"

extern const char *clientInit();
extern const char *clientOpen(char *argin, u32 inlen, u32 outlen);
extern const char *clientDelete(char *argin, u32 inlen, u32 outlen);
extern const char *clientAccess(char *argin, u32 inlen, u32 outlen);
extern const char *clientFullPathname(char *argin, u32 inlen, u32 outlen);
extern const char *clientRandomness(char *argin, u32 inlen, u32 outlen);
extern const char *clientSleep(char *argin, u32 inlen, u32 outlen);
extern const char *clientCurrentTime(char *argin, u32 inlen, u32 outlen);
extern const char *clientGetLastError(char *argin, u32 inlen, u32 outlen);
extern const char *clientCurrentTimeInt64(char *argin, u32 inlen, u32 outlen);
extern const char *clientWrite(char *argin, u32 inlen, u32 outlen);

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


int remoteInit(void) {
    char *p = (char *) clientInit();
    int *rc = (int *) (p);
    return *rc;
}
void Test_Init() {
    printf("%s\n", "---remoteInit:");
    printf("rc=%d\n", remoteInit());

}
/************** funtion unixOpen *****************************************/
static int remoteOpen(
        sqlite3_vfs *pVfs,           /* The VFS for which this is the xOpen method */
        const char *zPath,           /* Pathname of file to be opened */
        sqlite3_file *pFile,         /* The file descriptor to be filled in */
        int flags,                   /* Input flags to control the opening */
        int *pOutFlags               /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixOpen)];
    int rc = SQLITE_OK;

    unixOpenConvertArgInToChar(pVfs, zPath, pFile, flags, pOutFlags, argInChar);
    const char *argOutChar = clientOpen(argInChar, sizeof(ArgInUnixOpen), sizeof(ArgOutUnixOpen));
    unixOpenConvertCharToArgOut(argOutChar, pFile, pOutFlags, &rc);

    return rc;
}
void Test_Open() {
    unixFile fi;
    int outFlag = 2;
    int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000004 & 0x87f7f, &outFlag);

    printf("%s\n", "---remoteOpen:");
    printf("f.h = %d\n", fi.h);
    printf("f.path = %d\n", fi.zPath);
    printf("outFlag = %d\n", outFlag);
    printf("rc = %d\n", rc);
}
/************** funtion unixDelete *****************************************/
static int remoteDelete(
        sqlite3_vfs *NotUsed,     /* VFS containing this as the xDelete method */
        const char *zPath,        /* Name of file to be deleted */
        int dirSync             /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixDelete)];
    int rc = SQLITE_OK;

    unixDeleteConvertArgInToChar(NotUsed, zPath, dirSync, argInChar);
    const char *argOutChar = clientDelete(argInChar, sizeof(ArgInUnixDelete), sizeof(ArgOutUnixDelete));
    unixDeleteConvertCharToArgOut(argOutChar, &rc);

    return rc;
}
void Test_Delete() {
    int rc = remoteDelete(&aVfs[0], "./remote.db", 1);

    printf("%s\n", "---remoteDelete:");
    printf("rc = %d\n", rc);
}
/************** funtion unixAccess *****************************************/
static int remoteAccess(
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut            /* Write result boolean here */
) {
    char argInChar[sizeof(ArgInUnixAccess)];
    int rc = SQLITE_OK;

    unixAccessConvertArgInToChar(NotUsed, zPath, flags, pResOut, argInChar);
    const char *argOutChar = clientAccess(argInChar, sizeof(ArgInUnixAccess), sizeof(ArgOutUnixAccess));
    unixAccessConvertCharToArgOut(argOutChar, pResOut, &rc);

    return rc;
}
void Test_Access() {
    int res;
    int rc = remoteAccess(&aVfs[0], "./remote.db", 0, &res);

    printf("%s\n", "---remoteAccess:");
    printf("pResOut = %d\n", res);
    printf("rc = %d\n", rc);
}
/************** funtion unixFullPathname *****************************************/
static int remoteFullPathname(
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        const char *zPath,            /* Possibly relative input path */
        int nOut,                     /* Size of output buffer in bytes */
        char *zOut
) {
    char argInChar[sizeof(ArgInUnixFullPathname)];
    int rc = SQLITE_OK;

    unixFullPathnameConvertArgInToChar(pVfs, zPath, nOut, zOut, argInChar);
    const char *argOutChar = clientFullPathname(argInChar, sizeof(ArgInUnixFullPathname),
                                                sizeof(ArgOutUnixFullPathname));
    unixFullPathnameConvertCharToArgOut(argOutChar, zOut, &rc);

    return rc;
}
void Test_FullPathname() {
    char zOut[512];
    int rc = remoteFullPathname(&aVfs[0], "remote.db", 1, zOut);

    printf("%s\n", "---remoteFullPathname:");
    printf("zOut:%s\n", zOut);
    printf("rc = %d\n", rc);
}
/************** funtion unixRandomness *****************************************/
static int remoteRandomness(sqlite3_vfs *NotUsed, int nBuf, char *zBuf) {
    char argInChar[sizeof(ArgInUnixRandomness)];
    int rc = SQLITE_OK;

    unixRandomnessConvertArgInToChar(NotUsed, nBuf, zBuf, argInChar);
    const char *argOutChar = clientRandomness(argInChar, sizeof(ArgInUnixRandomness), sizeof(ArgOutUnixRandomness));
    unixRandomnessConvertCharToArgOut(argOutChar, zBuf, &rc);
    return rc;
}
void Test_Randomness() {
    char zBuf[256];
    int rc = remoteRandomness(&aVfs[0], 256, zBuf);

    printf("%s\n", "---remoteRandomness:");
    printf("zBuf:%s\n", zBuf);
    printf("rc = %d\n", rc);
}
/************** funtion unixSleep *****************************************/
static int remoteSleep(sqlite3_vfs *NotUsed, int microseconds) {
    char argInChar[sizeof(ArgInUnixSleep)];
    int rc = SQLITE_OK;

    unixSleepConvertArgInToChar(NotUsed, microseconds, argInChar);
    const char *argOutChar = clientSleep(argInChar, sizeof(ArgInUnixSleep), sizeof(ArgOutUnixSleep));
    unixSleepConvertCharToArgOut(argOutChar, &rc);

    return rc;
}
void Test_Sleep() {
    int rc = remoteSleep(&aVfs[0], 1000);

    printf("%s\n", "---remoteSleep:");
    printf("rc = %d\n", rc);
}
/************** funtion unixCurrentTime *****************************************/
static int remoteCurrentTime(sqlite3_vfs *NotUsed, double *prNow) {
    char argInChar[sizeof(ArgInUnixCurrentTime)];
    int rc = SQLITE_OK;

    unixCurrentTimeConvertArgInToChar(NotUsed, prNow, argInChar);
    const char *argOutChar = clientCurrentTime(argInChar, sizeof(ArgInUnixCurrentTime), sizeof(ArgOutUnixCurrentTime));
    unixCurrentTimeConvertCharToArgOut(argOutChar, prNow, &rc);

    return rc;
}
void Test_CurrentTime() {
    double prNow;
    int rc = remoteCurrentTime(&aVfs[0], &prNow);

    printf("%s\n", "---remoteCurrentTime:");
    printf("prNow:%f\n", prNow);
    printf("rc = %d\n", rc);

}
/************** funtion unixGetLastError *****************************************/
static int remoteGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3) {
    char argInChar[sizeof(ArgInUnixGetLastError)];
    int rc_errno = 0;

    unixGetLastErrorConvertArgInToChar(NotUsed, NotUsed2, NotUsed3, argInChar);
    const char *argOutChar = clientGetLastError(argInChar, sizeof(ArgInUnixGetLastError),
                                                sizeof(ArgOutUnixGetLastError));
    unixGetLastErrorConvertCharToArgOut(argOutChar, &rc_errno);
    return rc_errno;
}
void Test_GetLastError() {
    int rc_errno = remoteGetLastError(&aVfs[0], 0, "0");
    printf("%s\n", "---remoteGetLastError:");
    printf("rc_errno = %d\n", rc_errno);
}
/************** funtion unixCurrentTimeInt64 *****************************************/
static int remoteCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    char argInChar[sizeof(ArgInCurrentTimeInt64)];
    int rc = SQLITE_OK;

    unixCurrentTimeInt64ConvertArgInToChar(NotUsed, piNow, argInChar);
    const char *argOutChar = clientCurrentTimeInt64(argInChar, sizeof(ArgInCurrentTimeInt64),
                                                    sizeof(struct ArgOutCurrentTimeInt64));
    unixCurrentTimeInt64ConvertCharToArgOut(argOutChar, piNow, &rc);
    return rc;
}
void Test_CurrentTimeInt64() {
    sqlite3_int64 piNow;
    int rc = remoteCurrentTimeInt64(&aVfs[0], &piNow);

    printf("%s\n", "---remoteCurrentTimeInt64:");
    printf("piNow = %d\n", piNow);
    printf("rc = %d\n", rc);
}

/************** funtion unixWrite *****************************************/
static int remoteWrite(
        sqlite3_file *id,
        const void *pBuf,
        int amt,
        sqlite3_int64 offset){
    char argInChar[sizeof(ArgInWrite)];
    int rc = SQLITE_OK;

    char *tempBuf = (char*)pBuf;
    unixWriteConvertArgInToChar(id, tempBuf, amt, offset, argInChar);
    const char *argOutChar = clientWrite(argInChar, sizeof(ArgInWrite), sizeof(ArgOutWrite));
    unixWriteConvertCharToArgOut(argInChar, id, tempBuf, &rc);
    pBuf = tempBuf;

    return rc;
}
void Test_Open_And_Write(){
    unixFile fi;
    int outFlag = 0;
    int rc = remoteOpen(&aVfs[0], "./remote.db", (sqlite3_file *) &fi, 0x00000002 & 0x87f7f, &outFlag);

    printf("%s\n", "---Test_Open_And_Write:");
    printf("%s\n", "---Open:");
    printf("f.h = %d\n", fi.h);
    printf("f.path = %d\n", fi.zPath);
    printf("outFlag = %d\n", outFlag);
    printf("rc = %d\n", rc);

    char pBuf[4] = {'a','b','c','d'};
    rc = remoteWrite((sqlite3_file *) &fi, pBuf,4,2);

    printf("%s\n", "---Write:");
    printf("f.h = %d\n", fi.h);
    // TODO: rc一直返回 4
    printf("rc = %d\n", rc);
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
//    Test_GetLastError();

//    Test_CurrentTimeInt64();
    Test_Open_And_Write();
    return 0;
}
