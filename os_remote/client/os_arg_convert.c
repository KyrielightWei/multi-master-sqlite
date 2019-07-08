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

///************** funtion unixAccess *****************************************/
//
//struct ArgInUnixAccess {
//    char path[512];
//    int flags;
//    int pResOut;
//};
//typedef struct ArgInUnixAccess ArgInUnixAccess;
//struct ArgOutUnixAccess {
//    int pResOut;
//    int rc;
//};
//typedef struct ArgOutUnixAccess ArgOutUnixAccess;
//
//void unixAccessConvertArgInToChar(  // client
//        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
//        const char *zPath,      /* Path of the file to examine */
//        int flags,              /* What do we want to learn about the zPath file? */
//        int *pResOut,
//        char *arg
//) {
//    ArgInUnixAccess *arg_struct = (ArgInUnixAccess *) arg;
//    strcpy(arg_struct->path, zPath);
//    arg_struct->flags = flags;
//    arg_struct->pResOut = *pResOut;
//};
//void unixAccessConvertCharToArgOut(        // client
//        const char *arg,
//        int *pResOut,
//        int *pRc
//) {
//    ArgOutUnixAccess *arg_struct = (ArgOutUnixAccess *) arg;
//    memcpy(pResOut, &arg_struct->pResOut, sizeof(int));
//    memcpy(pRc, &arg_struct->rc, sizeof(int));
//}
//void unixAccessConvertCharToArgIn(         // server
//        const char *arg,
//        sqlite3_vfs *NotUsed,   /* The VFS containing this xAccess method */
//        char *zPath,      /* Path of the file to examine */
//        int *flags,              /* What do we want to learn about the zPath file? */
//        int *pResOut
//) {
//    ArgInUnixAccess *arg_struct = (ArgInUnixAccess *) arg;
//    strcpy(zPath, arg_struct->path);
//    memcpy(flags, &arg_struct->flags, sizeof(int));
//    memcpy(pResOut, &arg_struct->pResOut, sizeof(int));
//}
//void unixAccessConvertArgOutToChar(          // server
//        int *pResOut,
//        int *pRc,
//        char *arg
//) {
//    ArgOutUnixAccess *arg_struct = (ArgOutUnixAccess *) arg;
//    memcpy(&arg_struct->pResOut, pResOut, sizeof(int));
//    memcpy(&arg_struct->rc, pRc, sizeof(int));
//}
//
///************** funtion unixFullPathname *****************************************/
////sqlite3_vfs *pVfs,            /* Pointer to vfs object */
////const char *zPath,            /* Possibly relative input path */
////int nOut,                     /* Size of output buffer in bytes */
////char *zOut
//
//struct ArgInUnixFullPathname {
//    char path[512];
//    int nOut;
//    char zOut[512];
//};
//typedef struct ArgInUnixFullPathname ArgInUnixFullPathname;
//struct ArgOutUnixFullPathname {
//    char zOut[512];
//    int rc;
//};
//typedef struct ArgOutUnixFullPathname ArgOutUnixFullPathname;
//
//void unixFullPathnameConvertArgInToChar(// client
//        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
//        const char *zPath,            /* Possibly relative input path */
//        int nOut,                     /* Size of output buffer in bytes */
//        char *zOut,
//        char *arg
//) {
//    ArgInUnixFullPathname *arg_struct = (ArgInUnixFullPathname *) arg;
//    strcpy(arg_struct->path, zPath);
//    arg_struct->nOut = nOut;
//    strcpy(arg_struct->zOut, zOut);
//}
//void unixFullPathnameConvertCharToArgOut( // client
//        const char *arg,
//        char *zOut,
//        int *pRc,
//) {
//    ArgOutUnixFullPathname *arg_struct = (ArgOutUnixFullPathname *) arg;
//    strcpy(zOut, arg_struct->zOut);
//    memcpy(pRc, &arg_struct->rc);
//}
//void unixFullPathnameConvertCharToArgIn( // server
//        const char *arg,
//        sqlite3_vfs *pVfs,            /* Pointer to vfs object */
//        char *zPath,            /* Possibly relative input path */
//        int *nOut,                     /* Size of output buffer in bytes */
//        char *zOut
//) {
//    ArgInUnixFullPathname *arg_struct = (ArgInUnixFullPathname *) arg;
//    strcpy(zPath, arg_struct->path);
//    memcpy(nOut, &arg_struct->nOut, sizeof(int));
//    memcpy(zOut, &arg_struct->zOut, sizeof(int));
//}
//void unixFullPathnameConvertArgOutToChar( //server
//        char *zOut,
//        int *pRc,
//        char *arg
//) {
//    ArgOutUnixFullPathname *arg_struct = (ArgOutUnixFullPathname *) arg;
//    strcpy(arg_struct->zOut, zOut);
//    memcpy(&arg_struct->rc, pRc, sizeof(int));
//}
//
///************** funtion unixRandomness *****************************************/
////unixRandomness(sqlite3_vfs *NotUsed, int nBuf, char *zBuf)
//
//struct ArgInUnixRandomness {
//    int nBuf;
//    char zBuf[512];
//};
//typedef struct ArgInUnixRandomness ArgInUnixRandomness;
//struct ArgOutUnixRandomness {
//    char zBuf[512];
//    int rc;
//};
//typedef struct ArgOutUnixRandomness ArgOutUnixRandomness;
//
//void nameConvertArgInToChar(sqlite3_vfs *NotUsed, int nBuf, char *zBuf, char *arg) {
//    ArgInUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
//    arg_struct->nBuf = nBuf;
//    strcpy(arg_struct->zBuf, zBuf);
//}
//void nameConvertCharToArgOut(const char *arg, char *zBuf, int *rc) {
//    ArgOutUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
//    strcpy(zBuf, arg_struct->zBuf);
//    memcpy(rc, &arg_struct->rc, sizeof(int));
//}
//void nameConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *nBuf, char *zBuf) {
//    ArgInUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
//    memcpy(nBuf, &arg_struct->zBuf, sizeof(int));
//    strcpy(zBuf, arg_struct->zBuf);
//}
//void nameConvertArgOutToChar(char *zBuf, int *rc, char *arg) {
//    ArgOutUnixRandomness *arg_struct = (ArgInUnixRandomness *) arg;
//    strcpy(arg_struct->zBuf, zBuf)
//    memcpy(&arg_struct->rc, rc, sizeof(int));
//}
//
///************** funtion unixSleep *****************************************/
////int unixSleep(sqlite3_vfs *NotUsed, int microseconds)
//struct ArgInUnixSleep {
//    int microseconds;
//};
//typedef struct ArgInUnixSleep ArgInUnixSleep;
//struct ArgOutUnixSleep {
//    int rc;
//};
//typedef struct ArgOutUnixSleep ArgOutUnixSleep;
//
//void unixSleepConvertArgInToChar(sqlite3_vfs *NotUsed, int microseconds, char *arg) {
//    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
//    arg_struct->microseconds = microseconds;
//
//}
//void unixSleepConvertCharToArgOut(const char *arg, int *pRc) {
//    ArgOutUnixSleep *arg_struct = (ArgOutUnixSleep *) arg;
//    memcpy(pRc, &arg_struct->rc, sizeof(int));
//}
//void unixSleepConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *microseconds) {
//    ArgInUnixSleep *arg_struct = (ArgInUnixSleep *) arg;
//    memcpy(microseconds, &arg_struct->microseconds, sizeof(int));
//
//}
//void unixSleepConvertArgOutToChar(int *pRc, char *arg) {
//    ArgOutUnixSleep *arg_struct = (ArgOutUnixSleep *) arg;
//    memcpy(pRc, &arg_struct->rc, sizeof(int));
//}
//
///************** funtion unixCurrentTime *****************************************/
////int unixCurrentTime(sqlite3_vfs *NotUsed, double *prNow)
//struct ArgInUnixCurrentTime {
//    double prNow;
//};
//typedef struct ArgInUnixCurrentTime ArgInUnixCurrentTime;
//struct ArgOutUnixCurrentTime {
//    double prNow;
//    int rc;
//};
//typedef struct ArgOutUnixCurrentTime ArgOutUnixCurrentTime;
//
//void nameConvertArgInToChar(sqlite3_vfs *NotUsed, double *prNow, char *arg) {
//    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
//    memcpy(&arg_struct->prNow, prNow, sizeof(int));
//}
//void nameConvertCharToArgOut(const char *arg, double *prNow, int *pRc) {
//    ArgOutUnixCurrentTime *arg_struct = (struct ArgOutUnixCurrentTime *) arg;
//    memcpy(prNow, &arg_struct->prNow, sizeof(double));
//    memcpy(pRc, &arg_struct->rc, sizeof(int));
//}
//void nameConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, double *prNow) {
//    ArgInUnixCurrentTime *arg_struct = (ArgInUnixCurrentTime *) arg;
//    memcpy(prNow, &arg_struct->prNow, sizeof(double));
//}
//void nameConvertArgOutToChar(double *prNow, int *pRc, char *arg) {
//    ArgOutUnixCurrentTime *arg_struct = (struct ArgOutUnixCurrentTime *) arg;
//    memcpy(&arg_struct->prNow, prNow, sizeof(double));
//    memcpy(&arg_struct->rc, pRc, sizeof(int));
//}
//
///************** funtion unixGetLastError *****************************************/
//struct ArgInUnixGetLastError {
//
//};
//typedef struct ArgInUnixGetLastError ArgInUnixGetLastError;
//struct ArgOutUnixGetLastError {
//    int rc;
//};
//typedef struct ArgOutUnixGetLastError ArgOutUnixGetLastError;



