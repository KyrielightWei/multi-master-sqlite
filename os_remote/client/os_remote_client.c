#include "os_arg_convert.c"

extern const char *ClientInit();
extern const char *ClientOpen(char *argin, u32 inlen, u32 outlen);
extern const char *ClientDelete(char *argin, u32 inlen, u32 outlen);

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


int RemoteInit(void) {
    char *p = (char *) ClientInit();
    int *rc = (int *) (p);
    return *rc;
}
void Test_Init() {
    printf("%s", "---RemoteInit:\n");
    printf("rc=%d\n", RemoteInit());

}


static int RemoteOpen(
        sqlite3_vfs *pVfs,           /* The VFS for which this is the xOpen method */
        const char *zPath,           /* Pathname of file to be opened */
        sqlite3_file *pFile,         /* The file descriptor to be filled in */
        int flags,                   /* Input flags to control the opening */
        int *pOutFlags               /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixOpen)];
    int rc = SQLITE_OK;

    unixOpenConvertArgInToChar(pVfs, zPath, pFile, flags, pOutFlags, argInChar);
    const char *argOutChar = ClientOpen(argInChar, sizeof(ArgInUnixOpen), sizeof(ArgOutUnixOpen));
    unixOpenConvertCharToArgOut(argOutChar, pFile, pOutFlags, &rc);

    return rc;
}
void Test_Open() {
    unixFile fi;
    int outFlag = 2;
    int rc = RemoteOpen(&aVfs[0], "./os_server_wrap.h", (sqlite3_file *) &fi, 0, &outFlag);

    printf("%s", "---RemoteOpen:\n");
    printf("f = %d\n", fi.h);
    printf("outFlag = %d\n", outFlag);
    printf("rc = %d\n", rc);
}

static int RemoteDelete(
        sqlite3_vfs *NotUsed,     /* VFS containing this as the xDelete method */
        const char *zPath,        /* Name of file to be deleted */
        int dirSync             /* Output flags returned to SQLite core */
) {
    char argInChar[sizeof(ArgInUnixDelete)];
    int rc = SQLITE_OK;

    unixDeleteConvertArgInToChar(&aVfs[0], "./os_server_wrap.h", dirSync, argInChar);
    const char *argOutChar = ClientDelete(argInChar, sizeof(ArgInUnixDelete), sizeof(ArgOutUnixDelete));
    unixDeleteConvertCharToArgOut(argOutChar,&rc);

    return rc;
}
void Test_Delete() {

    int rc = RemoteDelete(&aVfs[0], "./os_server_wrap.h", 1);

    printf("%s", "---RemoteDelete:\n");
    printf("rc = %d\n", rc);
}

int main() {
//    Test_Init();
    Test_Open();
    Test_Delete();

    return 0;
}
