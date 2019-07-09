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
void nameConvertArgInToChar() // client
void nameConvertCharToArgOut() // client
void nameConvertCharToArgIn() // server
void nameConvertArgOutToChar()  //server



}
 */

#include "sqlite3.c"

/************** funtion unixOpen *****************************************/
struct ArgInUnixOpen {
    //no vfs
    char path[512];
    char file_infor[sizeof(unixFile)];
    int in_flags;
    int out_flags;
};
typedef struct ArgInUnixOpen ArgInUnixOpen;

struct ArgOutUnixOpen {
    char file_infor[sizeof(unixFile)];
    int out_flags;
    int rc;
};
typedef struct ArgOutUnixOpen ArgOutUnixOpen;

void unixOpenConvertArgInToChar(
        sqlite3_vfs *pVfs,        //Client
        const char *zPath,
        sqlite3_file *pFile,
        int flags,
        int *pOutFlags,
        char *arg
) {
    ArgInUnixOpen *arg_struct = (ArgInUnixOpen *) arg;
    strcpy(arg_struct->path, zPath);
    memcpy(arg_struct->file_infor, pFile, sizeof(unixFile));
    arg_struct->in_flags = flags;
    arg_struct->out_flags = *pOutFlags;
    return;
}

void unixOpenConvertCharToArgOut(
        const char *arg,          // Client
        sqlite3_file *pFile,
        int *pOutFlags,
        int *pRc
) {
    ArgOutUnixOpen *arg_struct = (ArgOutUnixOpen *) arg;
    memcpy(pFile, arg_struct->file_infor, sizeof(unixFile));
    memcpy(pOutFlags, &arg_struct->out_flags, sizeof(int));
    memcpy(pRc, &arg_struct->rc, sizeof(int));

    return;
}

void unixOpenConvertCharToArgIn(    //server
        const char *arg,
        sqlite3_vfs *pVfs,
        char *zPath,
        sqlite3_file *pFile,
        int *flags,
        int *pOutFlags
) {
    ArgInUnixOpen *arg_struct = (ArgInUnixOpen *) arg;
    strcpy(zPath, arg_struct->path);
    memcpy(pFile, arg_struct->file_infor, sizeof(unixFile));
    memcpy(flags, &arg_struct->in_flags, sizeof(int));
    memcpy(pOutFlags, &arg_struct->out_flags, sizeof(int));

    return;
}

void unixOpenConvertArgOutToChar(         // server
        sqlite3_file *pFile,
        int *pOutFlags,
        int *pRc,
        char *arg
) {
    ArgOutUnixOpen *arg_struct = (ArgOutUnixOpen *) arg;
    memcpy(arg_struct->file_infor, pFile, sizeof(unixFile));
    memcpy(&arg_struct->out_flags, pOutFlags, sizeof(int));
    memcpy(&arg_struct->rc, pRc, sizeof(int));

    return;
}


/************** funtion unixDelete *****************************************/
struct ArgInUnixDelete {
    char path[512];
    int ditSync;
};
typedef struct ArgInUnixDelete ArgInUnixDelete;
struct ArgOutUnixDelete {
    int rc;
};
typedef struct ArgOutUnixDelete ArgOutUnixDelete;

void unixDeleteConvertArgInToChar(  // client
        sqlite3_vfs *NotUsed,
        const char *zPath,
        int dirSync,
        char *arg
) {
    ArgInUnixDelete *arg_struct = (ArgInUnixDelete *) arg;
    strcpy(arg_struct->path, zPath);
    arg_struct->ditSync = dirSync;
}

void unixDeleteConvertCharToArgOut(      //  client
        const char *arg,
        int *pRc
) {
    ArgOutUnixDelete *arg_struct = (ArgOutUnixDelete *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixDeleteConvertCharToArgIn(       //server
        const char *arg,
        sqlite3_vfs *NotUsed,
        char *zPath,
        int *dirSync

) {
    ArgInUnixDelete *arg_struct = (ArgInUnixDelete *) arg;
    strcpy(zPath, arg_struct->path);
    memcpy(dirSync, &arg_struct->ditSync, sizeof(int));
}
void unixDeleteConvertArgOutToChar(      //server
        int *pRC,
        char *arg
) {
    ArgOutUnixDelete *arg_struct = (ArgOutUnixDelete *) arg;
    memcpy(&arg_struct->rc, pRC, sizeof(int));
}

/************** funtion unixAccess *****************************************/
struct ArgInUnixAccess {
    char path[512];
    int flags;
    int pResOut;
};
typedef struct ArgInUnixAccess ArgInUnixAccess;
struct ArgOutUnixAccess {
    int pResOut;
    int rc;
};
typedef struct ArgOutUnixAccess ArgOutUnixAccess;

void unixAccessConvertArgInToChar(  // client
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        const char *zPath,      /* Path of the file to examine */
        int flags,              /* What do we want to learn about the zPath file? */
        int *pResOut,
        char *arg
) {
    ArgInUnixAccess *arg_struct = (ArgInUnixAccess *) arg;
    strcpy(arg_struct->path, zPath);
    arg_struct->flags = flags;
    arg_struct->pResOut = *pResOut;
};
void unixAccessConvertCharToArgOut(        // client
        const char *arg,
        int *pResOut,
        int *pRc
) {
    ArgOutUnixAccess *arg_struct = (ArgOutUnixAccess *) arg;
    memcpy(pResOut, &arg_struct->pResOut, sizeof(int));
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixAccessConvertCharToArgIn(         // server
        const char *arg,
        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
        char *zPath,      /* Path of the file to examine */
        int *flags,              /* What do we want to learn about the zPath file? */
        int *pResOut
) {
    ArgInUnixAccess *arg_struct = (ArgInUnixAccess *) arg;
    strcpy(zPath, arg_struct->path);
    memcpy(flags, &arg_struct->flags, sizeof(int));
    memcpy(pResOut, &arg_struct->pResOut, sizeof(int));
}
void unixAccessConvertArgOutToChar(          // server
        int *pResOut,
        int *pRc,
        char *arg
) {
    ArgOutUnixAccess *arg_struct = (ArgOutUnixAccess *) arg;
    memcpy(&arg_struct->pResOut, pResOut, sizeof(int));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}

/************** funtion unixFullPathname *****************************************/
//sqlite3_vfs *pVfs,            /* Pointer to vfs object */
//const char *zPath,            /* Possibly relative input path */
//int nOut,                     /* Size of output buffer in bytes */
//char *zOut

struct ArgInUnixFullPathname {
    char path[512];
    int nOut;
    char zOut[512];
};
typedef struct ArgInUnixFullPathname ArgInUnixFullPathname;
struct ArgOutUnixFullPathname {
    char zOut[512];
    int rc;
};
typedef struct ArgOutUnixFullPathname ArgOutUnixFullPathname;

void unixFullPathnameConvertArgInToChar(// client
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        const char *zPath,            /* Possibly relative input path */
        int nOut,                     /* Size of output buffer in bytes */
        char *zOut,
        char *arg
) {
    ArgInUnixFullPathname *arg_struct = (ArgInUnixFullPathname *) arg;
    strcpy(arg_struct->path, zPath);
    arg_struct->nOut = nOut;
    strcpy(arg_struct->zOut, zOut);
}
void unixFullPathnameConvertCharToArgOut( // client
        const char *arg,
        char *zOut,
        int *pRc
) {
    ArgOutUnixFullPathname *arg_struct = (ArgOutUnixFullPathname *) arg;
    strcpy(zOut, arg_struct->zOut);
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixFullPathnameConvertCharToArgIn( // server
        const char *arg,
        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
        char *zPath,            /* Possibly relative input path */
        int *nOut,                     /* Size of output buffer in bytes */
        char *zOut
) {
    ArgInUnixFullPathname *arg_struct = (ArgInUnixFullPathname *) arg;
    strcpy(zPath, arg_struct->path);
    memcpy(nOut, &arg_struct->nOut, sizeof(int));
    strcpy(zOut, arg_struct->zOut);
}
void unixFullPathnameConvertArgOutToChar( //server
        char *zOut,
        int *pRc,
        char *arg
) {
    ArgOutUnixFullPathname *arg_struct = (ArgOutUnixFullPathname *) arg;
    strcpy(arg_struct->zOut, zOut);
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}

/************** funtion unixRandomness *****************************************/
//unixRandomness(sqlite3_vfs *NotUsed, int nBuf, char *zBuf)

struct ArgInUnixRandomness {
    int nBuf;
    char zBuf[256];
};
typedef struct ArgInUnixRandomness ArgInUnixRandomness;
struct ArgOutUnixRandomness {
    char zBuf[256];
    int rc;
};
typedef struct ArgOutUnixRandomness ArgOutUnixRandomness;

void unixRandomnessConvertArgInToChar(sqlite3_vfs *NotUsed, int nBuf, char *zBuf, char *arg) {
    ArgInUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
    arg_struct->nBuf = nBuf;
    strcpy(arg_struct->zBuf, zBuf);
}
void unixRandomnessConvertCharToArgOut(const char *arg, char *zBuf, int *rc) {
    ArgOutUnixRandomness *arg_struct = (ArgOutUnixRandomness *) arg;
    strcpy(zBuf, arg_struct->zBuf);
    memcpy(rc, &arg_struct->rc, sizeof(int));
}
void unixRandomnessConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *nBuf, char *zBuf) {
    ArgInUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
    memcpy(nBuf, &arg_struct->zBuf, sizeof(int));
    strcpy(zBuf, arg_struct->zBuf);
}
void unixRandomnessConvertArgOutToChar(char *zBuf, int *rc, char *arg) {
    ArgOutUnixRandomness *arg_struct = (ArgOutUnixRandomness *) arg;
    strcpy(arg_struct->zBuf, zBuf);
    memcpy(&arg_struct->rc, rc, sizeof(int));
}

/************** funtion unixSleep *****************************************/
//int unixSleep(sqlite3_vfs *NotUsed, int microseconds)
struct ArgInUnixSleep {
    int microseconds;
};
typedef struct ArgInUnixSleep ArgInUnixSleep;
struct ArgOutUnixSleep {
    int rc;
};
typedef struct ArgOutUnixSleep ArgOutUnixSleep;

void unixSleepConvertArgInToChar(sqlite3_vfs *NotUsed, int microseconds, char *arg) {
    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
    arg_struct->microseconds = microseconds;

}
void unixSleepConvertCharToArgOut(const char *arg, int *pRc) {
    ArgOutUnixSleep *arg_struct = (ArgOutUnixSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixSleepConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *microseconds) {
    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
    memcpy(microseconds, &arg_struct->microseconds, sizeof(int));

}
void unixSleepConvertArgOutToChar(int *pRc, char *arg) {
    ArgOutUnixSleep *arg_struct = (ArgOutUnixSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}

/************** funtion unixCurrentTime *****************************************/
//int unixCurrentTime(sqlite3_vfs *NotUsed, double *prNow)
struct ArgInUnixCurrentTime {
    double prNow;
};
typedef struct ArgInUnixCurrentTime ArgInUnixCurrentTime;
struct ArgOutUnixCurrentTime {
    double prNow;
    int rc;
};
typedef struct ArgOutUnixCurrentTime ArgOutUnixCurrentTime;

void unixCurrentTimeConvertArgInToChar(sqlite3_vfs *NotUsed, double *prNow, char *arg) {
    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
    arg_struct->prNow = *prNow;
}
void unixCurrentTimeConvertCharToArgOut(const char *arg, double *prNow, int *pRc) {
    ArgOutUnixCurrentTime *arg_struct = (ArgOutUnixCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixCurrentTimeConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, double *prNow) {
    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
}
void unixCurrentTimeConvertArgOutToChar(double *prNow, int *pRc, char *arg) {
    ArgOutUnixCurrentTime *arg_struct = (ArgOutUnixCurrentTime *) arg;
    memcpy(&arg_struct->prNow, prNow, sizeof(double));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}

/************** funtion unixGetLastError *****************************************/
//static int unixGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3){
struct ArgInUnixGetLastError {
    int NotUsed2;
    char NotUsed3[2];
};
typedef struct ArgInUnixGetLastError ArgInUnixGetLastError;
struct ArgOutUnixGetLastError {
    int rc_errno;
};
typedef struct ArgOutUnixGetLastError ArgOutUnixGetLastError;

void unixGetLastErrorConvertArgInToChar(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3, char *arg) {
    ArgInUnixGetLastError *arg_struct = (ArgInUnixGetLastError *) arg;
    arg_struct->NotUsed2 = NotUsed2;
    strcpy(arg_struct->NotUsed3, NotUsed3);
}
void unixGetLastErrorConvertCharToArgOut(const char *arg, int *pRc_errno) {
    ArgOutUnixGetLastError *arg_struct = (ArgOutUnixGetLastError *) arg;
    memcpy(pRc_errno, &arg_struct->rc_errno, sizeof(int));
}
void unixGetLastErrorConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *NotUsed2, char *NotUsed3) {
    ArgInUnixGetLastError *arg_struct = (ArgInUnixGetLastError *) arg;
    memcpy(NotUsed2, &arg_struct->NotUsed2, sizeof(int));
    strcpy(NotUsed3, arg_struct->NotUsed3);
}
void unixGetLastErrorConvertArgOutToChar(int *pRc_errno, char *arg) {
    ArgOutUnixGetLastError *arg_struct = (ArgOutUnixGetLastError *) arg;
    memcpy(&arg_struct->rc_errno, pRc_errno, sizeof(int));
}


/************** funtion unixCurrentTimeInt64 *****************************************/
//static int unixCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow)
struct ArgInCurrentTimeInt64 {
    sqlite3_int64 piNow;
};
typedef struct ArgInCurrentTimeInt64 ArgInCurrentTimeInt64;

struct ArgOutCurrentTimeInt64 {
    sqlite3_int64 piNow;
    int rc;
};
typedef struct ArgOutCurrentTimeInt64 ArgOutCurrentTimeInt64;

void unixCurrentTimeInt64ConvertArgInToChar(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow, char *arg) {
    ArgInCurrentTimeInt64 *arg_struct = (ArgInCurrentTimeInt64 *) arg;
    arg_struct->piNow = *piNow;
}
void unixCurrentTimeInt64ConvertCharToArgOut(const char *arg, sqlite3_int64 *piNow, int *pRc) {
    ArgOutCurrentTimeInt64 *arg_struct = (ArgOutCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
    memcpy(pRc, &arg_struct->rc, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    ArgInCurrentTimeInt64 *arg_struct = (ArgInCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertArgOutToChar(sqlite3_int64 *piNow, int *pRc, char *arg) {
    ArgOutCurrentTimeInt64 *arg_struct = (ArgOutCurrentTimeInt64 *) arg;
    memcpy(&arg_struct->piNow, piNow, sizeof(sqlite3_int64));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}
/************** funtion unixWrite *****************************************/
//static int unixWrite(
//        sqlite3_file *id,
//        const void *pBuf,
//        int amt,
//        sqlite3_int64 offset
//)
struct ArgInWrite {
    char file_infor[sizeof(unixFile)];
    char pBuf[8192];
    int amt;
    sqlite3_int64 offset;
};
typedef struct ArgInWrite ArgInWrite;

struct ArgOutWrite {
    char file_infor[sizeof(unixFile)];
    char pBuf[8192];
    int rc;
};
typedef struct ArgOutWrite ArgOutWrite;

void unixWriteConvertArgInToChar(
        sqlite3_file *id,
        char *pBuf,
        int amt,
        sqlite3_int64 offset,
        char *arg) {
    ArgInWrite *arg_struct = (ArgInWrite *) arg;
    memcpy(arg_struct->file_infor, id, sizeof(unixFile));
    strcpy(arg_struct->pBuf, pBuf);
    arg_struct->amt = amt;
    arg_struct->offset = offset;
}
void unixWriteConvertCharToArgOut(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *pRc
) {
    ArgOutWrite *arg_struct = (ArgOutWrite *) arg;
    memcpy(id, arg_struct->file_infor, sizeof(unixFile));
    strcpy(pBuf, arg_struct->pBuf);
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixWriteConvertCharToArgIn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        sqlite3_int64 *offset
) {
    ArgInWrite *arg_struct = (ArgInWrite *) arg;
    memcpy(id, arg_struct->file_infor, sizeof(unixFile));
    strcpy(pBuf, arg_struct->pBuf);
    memcpy(amt, &arg_struct->amt, sizeof(int));
    memcpy(offset, &arg_struct->offset, sizeof(sqlite3_int64));
}
void unixWriteConvertArgOutToChar(
        sqlite3_file *id,
        char *pBuf,
        int *pRc,
        char *arg
) {
    ArgOutWrite *arg_struct = (ArgOutWrite *) arg;
    memcpy(arg_struct->file_infor, id, sizeof(unixFile));
    strcpy(arg_struct->pBuf, pBuf);
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}