//转换函数定义

/**
 * 客户端：
 *     1. 包装函数 —— 返回值：const char * ，服务端的回复 ；参数：char * ，u32 ,u32 参数打包的结构体和长度,返回值结构体的长度
 *     2. 转换函数 —— 参数->结构体：拷贝，指针转换为对应大小的char数组存储 ； 结构体->返回值 ： 拷贝，从包装函数返回的值拷贝到原来的参数指示的空间
 *     过程：客户端本地函数内 -  转换函数：参数->结构体
 *                          - 包装函数
 *                          - 转换函数：结构体->返回值
 * 
 * 传输内容： 客户端->服务端 ： 参数打包的结构体 返回值打包结构体的大小
 *           服务端->客户端 :  返回值打包的结构体
 * 
 * 服务端：
 *     1. 包装函数 —— 返回值：无 ； 参数：const char * ，char * 客户端发来的参数的结构体的指针 和 分配的返回值空间的指针
 *     2. 转换函数 —— 结构体->参数: 拷贝到包装函数分配的参数变量空间，便于修改 ； 返回值->结构体 ： 拷贝， 拷贝到包装函数给出的返回值指针
 *      过程：包装函数内 - 转换函数：结构体->参数
 *                      - 服务端本地函数
 *                      - 转换函数：返回值->结构体
 */

/**
 * 转换函数 命名：函数名_convertArgToChar(函数参数,char *) //转换前预先分配结构体占用的char数组
 *               函数名_convertCharToArg(char * ,函数参数) //转换前预先分配函数参数占用的空间，传入指针
 *              当转换内容为返回值时，将Return替换Arg
 */

//void nameConvertArgInToChar() // client
//void nameConvertCharToReturn() // client
//void nameConvertCharToArgIn() // server
//void nameConvertReturnToChar()  //server


#include "sqlite3.c"


#define SIZE_UNIXFILE sizeof(unixFile)
#define SQLITE_MAX_DEFAULT_PAGE_SIZE 8192

void print_char(char *p, int len) {
    int i;
    printf("%s\n", p);
    for (i = 0; i < len; i++) {
        printf("%d", p[i]);
        if (0 == (i + 1) % 100) {
            printf("\n");
        }
    }
    printf("\n");
}


/************** function unixOpen *****************************************/
struct ArgInUnixOpen {
    //no vfs
    char path[512];
    char file_infor[SIZE_UNIXFILE];
    int in_flags;
    int out_flags;
};
typedef struct ArgInUnixOpen ArgInUnixOpen;

struct ReturnUnixOpen {
    char file_infor[SIZE_UNIXFILE];
    int out_flags;
    int rc;
};
typedef struct ReturnUnixOpen ReturnUnixOpen;


void unixOpenConvertArgInToChar(        //Client
        sqlite3_vfs *pVfs,
        const char *zPath,
        sqlite3_file *pFile,
        int flags,
        int *pOutFlags,
        char *arg
) {
    strcpy(arg, zPath);
    memcpy(arg + 512, pFile, SIZE_UNIXFILE);
    memcpy(arg + 512 + SIZE_UNIXFILE, &flags, sizeof(int));
    memcpy(arg + 512 + SIZE_UNIXFILE + sizeof(int), pOutFlags, sizeof(int));
}

void unixOpenConvertCharToReturn(         // Client
        const char *arg,
        sqlite3_file *pFile,
        int *pOutFlags,
        int *pRc
) {
    memcpy(pFile, arg, SIZE_UNIXFILE);
    memcpy(pOutFlags, arg + SIZE_UNIXFILE, sizeof(int));
    memcpy(pRc, arg + SIZE_UNIXFILE + sizeof(int), sizeof(int));
}

void unixOpenConvertCharToArgIn(    //server
        const char *arg,
        sqlite3_vfs *pVfs,
        char *zPath,
        sqlite3_file *pFile,
        int *flags,
        int *pOutFlags
) {
    strcpy(zPath, arg);
    memcpy(pFile, arg + 512, SIZE_UNIXFILE);
    memcpy(flags, arg + 512 + SIZE_UNIXFILE, sizeof(int));
    memcpy(pOutFlags, arg + 512 + SIZE_UNIXFILE + sizeof(int), sizeof(int));
}

void unixOpenConvertReturnToChar(         // server
        sqlite3_file *pFile,
        int *pOutFlags,
        int *pRc,
        char *arg
) {
    memcpy(arg, pFile, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pOutFlags, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + sizeof(int), pRc, sizeof(int));
}


/************** function unixDelete *****************************************/
struct ArgInUnixDelete {
    char path[512];
    int ditSync;
};
typedef struct ArgInUnixDelete ArgInUnixDelete;
struct ReturnUnixDelete {
    int rc;
};
typedef struct ReturnUnixDelete ReturnUnixDelete;

void unixDeleteConvertArgInToChar(  // client
        sqlite3_vfs *NotUsed,
        const char *zPath,
        int dirSync,
        char *arg
) {
    strcpy(arg, zPath);
    memcpy(arg + 512, &dirSync, sizeof(int));
}

void unixDeleteConvertCharToReturn(      //  client
        const char *arg,
        int *pRc
) {
    memcpy(pRc, arg, sizeof(int));
}
void unixDeleteConvertCharToArgIn(       //server
        const char *arg,
        sqlite3_vfs *NotUsed,
        char *zPath,
        int *dirSync

) {
    strcpy(zPath, arg);
    memcpy(dirSync, arg + 512, sizeof(int));
}
void unixDeleteConvertReturnToChar(      //server
        int *pRC,
        char *arg
) {
    memcpy(arg, pRC, sizeof(int));
}

/************** function unixAccess *****************************************/
struct ArgInUnixAccess {
    char path[512];
    int flags;
    int pResOut;
};
typedef struct ArgInUnixAccess ArgInUnixAccess;
struct ReturnUnixAccess {
    int pResOut;
    int rc;
};
typedef struct ReturnUnixAccess ReturnUnixAccess;

void unixAccessConvertArgInToChar(  // client
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut,
        char *arg
) {
    strcpy(arg, zPath);
    memcpy(arg + 512, &flags, sizeof(int));
    memcpy(arg + 512 + sizeof(int), pResOut, sizeof(int));
};
void unixAccessConvertCharToReturn(        // client
        const char *arg,
        int *pResOut,
        int *pRc
) {
    memcpy(pResOut, arg, sizeof(int));
    memcpy(pRc, arg + sizeof(int), sizeof(int));
}
void unixAccessConvertCharToArgIn(         // server
        const char *arg,
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        char *zPath,      /* Path of the file to examine */
        int *flags,              /* What do we want to learn about the zPath file? */
        int *pResOut
) {
    strcpy(zPath, arg);
    memcpy(flags, arg + 512, sizeof(int));
    memcpy(pResOut, arg + 512 + sizeof(int), sizeof(int));
}
void unixAccessConvertReturnToChar(          // server
        int *pResOut,
        int *pRc,
        char *arg
) {
    memcpy(arg, pResOut, sizeof(int));
    memcpy(arg + sizeof(int), pRc, sizeof(int));
}

/************** function unixFullPathname *****************************************/

struct ArgInUnixFullPathname {
    char path[512];
    int nOut;
    char zOut[512];
};
typedef struct ArgInUnixFullPathname ArgInUnixFullPathname;
struct ReturnUnixFullPathname {
    char zOut[512];
    int rc;
};
typedef struct ReturnUnixFullPathname ReturnUnixFullPathname;

void unixFullPathnameConvertArgInToChar(// client
        sqlite3_vfs *pVfs,
        const char *zPath,
        int nOut,
        char *zOut,
        char *arg
) {
    strcpy(arg, zPath);
    memcpy(arg + 512, &nOut, sizeof(int));
//    memcpy(arg + 512 + sizeof(int), zOut, 512);
}
void unixFullPathnameConvertCharToReturn( // client
        const char *arg,
        char *zOut,
        int *pRc
) {
    strcpy(zOut, arg);
    memcpy(pRc, arg + 512, sizeof(int));
}
void unixFullPathnameConvertCharToArgIn( // server
        const char *arg,
        sqlite3_vfs *pVfs,
        char *zPath,
        int *nOut,
        char *zOut
) {
    strcpy(zPath, arg);
    memcpy(nOut, arg + 512, sizeof(int));
//    strcpy(zOut, arg + 512 + sizeof(int));
}
void unixFullPathnameConvertReturnToChar( //server
        char *zOut,
        int *pRc,
        char *arg
) {
    strcpy(arg, zOut);
    memcpy(arg + 512, pRc, sizeof(int));
}


/*
 * 以下几个函数直接调用客户端函数，不用调用remote：
 *      unixRandomness
 *      unixSleep
 *      unixCurrentTime
 *      unixCurrentTimeInt64
 * 所以注释掉了
 */
///////////////// function unixRandomness //////////////////////////////////////////
/*
struct ArgInUnixRandomness {
    int nBuf;
    char zBuf[256];
};
typedef struct ArgInUnixRandomness ArgInUnixRandomness;
struct ReturnUnixRandomness {
    char zBuf[256];
    int rc;
};
typedef struct ReturnUnixRandomness ReturnUnixRandomness;

void unixRandomnessConvertArgInToChar(sqlite3_vfs *NotUsed, int nBuf, char *zBuf, char *arg) {
    memcpy(arg, &nBuf, sizeof(int));
    strcpy(arg + sizeof(int), zBuf);
}
void unixRandomnessConvertCharToReturn(const char *arg, char *zBuf, int *rc) {
    strcpy(zBuf, arg);
    memcpy(rc, arg + 256, sizeof(int));
}
void unixRandomnessConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *nBuf, char *zBuf) {
    memcpy(nBuf, arg, sizeof(int));
    strcpy(zBuf, arg + sizeof(int));
}
void unixRandomnessConvertReturnToChar(char *zBuf, int *rc, char *arg) {
    strcpy(arg, zBuf);
    memcpy(arg + 256, rc, sizeof(int));
}

///////////////// function unixSleep  //////////////////////////////////////////
struct ArgInUnixSleep {
    int microseconds;
};
typedef struct ArgInUnixSleep ArgInUnixSleep;
struct ReturnUnixSleep {
    int rc;
};
typedef struct ReturnUnixSleep ReturnUnixSleep;

void unixSleepConvertArgInToChar(sqlite3_vfs *NotUsed, int microseconds, char *arg) {
    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
    arg_struct->microseconds = microseconds;

}
void unixSleepConvertCharToReturn(const char *arg, int *pRc) {
    ReturnUnixSleep *arg_struct = (ReturnUnixSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixSleepConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *microseconds) {
    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
    memcpy(microseconds, &arg_struct->microseconds, sizeof(int));

}
void unixSleepConvertReturnToChar(int *pRc, char *arg) {
    ReturnUnixSleep *arg_struct = (ReturnUnixSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}


///////////////// function unixCurrentTime  //////////////////////////////////////////
struct ArgInUnixCurrentTime {
    double prNow;
};
typedef struct ArgInUnixCurrentTime ArgInUnixCurrentTime;
struct ReturnUnixCurrentTime {
    double prNow;
    int rc;
};
typedef struct ReturnUnixCurrentTime ReturnUnixCurrentTime;

void unixCurrentTimeConvertArgInToChar(sqlite3_vfs *NotUsed, double *prNow, char *arg) {
    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
    arg_struct->prNow = *prNow;
}
void unixCurrentTimeConvertCharToReturn(const char *arg, double *prNow, int *pRc) {
    ReturnUnixCurrentTime *arg_struct = (ReturnUnixCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixCurrentTimeConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, double *prNow) {
    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
}
void unixCurrentTimeConvertReturnToChar(double *prNow, int *pRc, char *arg) {
    ReturnUnixCurrentTime *arg_struct = (ReturnUnixCurrentTime *) arg;
    memcpy(&arg_struct->prNow, prNow, sizeof(double));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}

///////////////// function unixCurrentTimeInt64  //////////////////////////////////////////

//static int unixCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow)
struct ArgInUnixCurrentTimeInt64 {
    sqlite3_int64 piNow;
};
typedef struct ArgInUnixCurrentTimeInt64 ArgInUnixCurrentTimeInt64;

struct ReturnUnixCurrentTimeInt64 {
    sqlite3_int64 piNow;
    int rc;
};
typedef struct ReturnUnixCurrentTimeInt64 ReturnUnixCurrentTimeInt64;

void unixCurrentTimeInt64ConvertArgInToChar(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow, char *arg) {
    ArgInUnixCurrentTimeInt64 *arg_struct = (ArgInUnixCurrentTimeInt64 *) arg;
    arg_struct->piNow = *piNow;
}
void unixCurrentTimeInt64ConvertCharToReturn(const char *arg, sqlite3_int64 *piNow, int *pRc) {
    ReturnUnixCurrentTimeInt64 *arg_struct = (ReturnUnixCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
    memcpy(pRc, &arg_struct->rc, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    ArgInUnixCurrentTimeInt64 *arg_struct = (ArgInUnixCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertReturnToChar(sqlite3_int64 *piNow, int *pRc, char *arg) {
    ReturnUnixCurrentTimeInt64 *arg_struct = (ReturnUnixCurrentTimeInt64 *) arg;
    memcpy(&arg_struct->piNow, piNow, sizeof(sqlite3_int64));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}
*/


/////////////// function unixGetLastError //////////////////////////////////////////
//static int unixGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3){
struct ArgInUnixGetLastError {
    int NotUsed2;
    char NotUsed3[2];
};
typedef struct ArgInUnixGetLastError ArgInUnixGetLastError;
struct ReturnUnixGetLastError {
    int rc_errno;
};
typedef struct ReturnUnixGetLastError ReturnUnixGetLastError;

void unixGetLastErrorConvertArgInToChar(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3, char *arg) {
//    memcpy(arg, &NotUsed2, sizeof(int));
//    memcpy(arg+ sizeof(int), NotUsed3, sizeof(char));
}
void unixGetLastErrorConvertCharToReturn(const char *arg, int *pRc_errno) {
    memcpy(pRc_errno, arg, sizeof(int));
}
void unixGetLastErrorConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *NotUsed2, char *NotUsed3) {

}
void unixGetLastErrorConvertReturnToChar(int *pRc_errno, char *arg) {
    memcpy(arg, pRc_errno, sizeof(int));
}

/////////////// function unixRead  //////////////////////////////////////////
struct ArgInUnixRead {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[SQLITE_MAX_DEFAULT_PAGE_SIZE];
    int amt;
    sqlite3_int64 offset;
};
typedef struct ArgInUnixRead ArgInUnixRead;

struct ReturnUnixRead {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[SQLITE_MAX_DEFAULT_PAGE_SIZE];
    int rc;
};
typedef struct ReturnUnixRead ReturnUnixRead;

void unixReadConvertArgInToChar(    // client
        sqlite3_file *id,
        char *pBuf,
        int amt,
        sqlite3_int64 offset,
        char *arg
) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pBuf, amt);
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, &amt, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE + sizeof(int), &offset, sizeof(sqlite3_int64));
}
void unixReadConvertCharToReturn(    // client
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        int *pRc
) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(pRc, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, sizeof(int));
}

void unixReadConvertCharToArgIn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        sqlite3_int64 *offset
) {

    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(amt, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, sizeof(int));
    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(offset, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE + sizeof(int), sizeof(sqlite3_int64));
}
void unixReadConvertReturnToChar(
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        int *pRc,
        char *arg
) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pBuf, *amt);
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, pRc, sizeof(int));
}


/////////////// function unixWrite  //////////////////////////////////////////
struct ArgInWrite {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[SQLITE_MAX_DEFAULT_PAGE_SIZE];
    int amt;
    sqlite3_int64 offset;
};
typedef struct ArgInWrite ArgInWrite;

struct ReturnWrite {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[SQLITE_MAX_DEFAULT_PAGE_SIZE];
    int rc;
};
typedef struct ReturnWrite ReturnWrite;

void unixWriteConvertArgInToChar(
        sqlite3_file *id,
        char *pBuf,
        int amt,
        sqlite3_int64 offset,
        char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pBuf, amt);
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, &amt, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE + sizeof(int), &offset, sizeof(sqlite3_int64));
}
void unixWriteConvertCharToReturn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        int *pRc
) {
    memcpy(id, arg, SIZE_UNIXFILE);
//    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(pRc, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, sizeof(int));
}
void unixWriteConvertCharToArgIn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        sqlite3_int64 *offset
) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(amt, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, sizeof(int));
    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(offset, arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE + sizeof(int), sizeof(sqlite3_int64));
}
void unixWriteConvertReturnToChar(
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        int *pRc,
        char *arg
) {
    memcpy(arg, id, SIZE_UNIXFILE);
//    memcpy(arg + SIZE_UNIXFILE, pBuf, *amt);
    memcpy(arg + SIZE_UNIXFILE + SQLITE_MAX_DEFAULT_PAGE_SIZE, pRc, sizeof(int));
}