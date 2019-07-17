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


//#include "sqlite3.c"


#define SIZE_UNIXFILE sizeof(unixFile)
#define MAX_SIZE 8192

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

/////////////// sqlite3_os_init ////////////////////////////////////////////
struct ArgInInit {

};
typedef struct ArgInInit ArgInInit;

struct ReturnInit {
    int rc;
};

void sqlite3_os_initConvertCharToReturn(const char *arg, int *rc) {
    memcpy(rc, arg, sizeof(int));
}

void sqlite3_os_initConvertReturnToChar(int *rc, char *arg) {
    memcpy(arg, rc, sizeof(int));
}

/************** function unixOpen *****************************************/
struct ArgInOpen {
    //no vfs
    char path[512];
    char file_infor[SIZE_UNIXFILE];
    int in_flags;
    int out_flags;
};
typedef struct ArgInOpen ArgInOpen;

struct ReturnOpen {
    char file_infor[SIZE_UNIXFILE];
    int out_flags;
    int rc;
};
typedef struct ReturnOpen ReturnOpen;


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
struct ArgInDelete {
    char path[512];
    int ditSync;
};
typedef struct ArgInDelete ArgInDelete;
struct ReturnDelete {
    int rc;
};
typedef struct ReturnDelete ReturnDelete;

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
struct ArgInAccess {
    char path[512];
    int flags;
    int pResOut;
};
typedef struct ArgInAccess ArgInAccess;
struct ReturnAccess {
    int pResOut;
    int rc;
};
typedef struct ReturnAccess ReturnAccess;

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

struct ArgInFullPathname {
    char path[512];
    int nOut;
    char zOut[512];
};
typedef struct ArgInFullPathname ArgInFullPathname;
struct ReturnFullPathname {
    char zOut[512];
    int rc;
};
typedef struct ReturnFullPathname ReturnFullPathname;

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
struct ArgInRandomness {
    int nBuf;
    char zBuf[256];
};
typedef struct ArgInRandomness ArgInRandomness;
struct ReturnRandomness {
    char zBuf[256];
    int rc;
};
typedef struct ReturnRandomness ReturnRandomness;

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
struct ArgInSleep {
    int microseconds;
};
typedef struct ArgInSleep ArgInSleep;
struct ReturnSleep {
    int rc;
};
typedef struct ReturnSleep ReturnSleep;

void unixSleepConvertArgInToChar(sqlite3_vfs *NotUsed, int microseconds, char *arg) {
    ArgInSleep *arg_struct = (ArgInSleep *) arg;
    arg_struct->microseconds = microseconds;

}
void unixSleepConvertCharToReturn(const char *arg, int *pRc) {
    ReturnSleep *arg_struct = (ReturnSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixSleepConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, int *microseconds) {
    ArgInSleep *arg_struct = (ArgInSleep *) arg;
    memcpy(microseconds, &arg_struct->microseconds, sizeof(int));

}
void unixSleepConvertReturnToChar(int *pRc, char *arg) {
    ReturnSleep *arg_struct = (ReturnSleep *) arg;
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}


///////////////// function unixCurrentTime  //////////////////////////////////////////
struct ArgInCurrentTime {
    double prNow;
};
typedef struct ArgInCurrentTime ArgInCurrentTime;
struct ReturnCurrentTime {
    double prNow;
    int rc;
};
typedef struct ReturnCurrentTime ReturnCurrentTime;

void unixCurrentTimeConvertArgInToChar(sqlite3_vfs *NotUsed, double *prNow, char *arg) {
    ArgInCurrentTime *arg_struct = (ArgInCurrentTime *) arg;
    arg_struct->prNow = *prNow;
}
void unixCurrentTimeConvertCharToReturn(const char *arg, double *prNow, int *pRc) {
    ReturnCurrentTime *arg_struct = (ReturnCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
    memcpy(pRc, &arg_struct->rc, sizeof(int));
}
void unixCurrentTimeConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, double *prNow) {
    ArgInCurrentTime *arg_struct = (ArgInCurrentTime *) arg;
    memcpy(prNow, &arg_struct->prNow, sizeof(double));
}
void unixCurrentTimeConvertReturnToChar(double *prNow, int *pRc, char *arg) {
    ReturnCurrentTime *arg_struct = (ReturnCurrentTime *) arg;
    memcpy(&arg_struct->prNow, prNow, sizeof(double));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}

///////////////// function unixCurrentTimeInt64  //////////////////////////////////////////

//static int unixCurrentTimeInt64(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow)
struct ArgInCurrentTimeInt64 {
    sqlite3_int64 piNow;
};
typedef struct ArgInCurrentTimeInt64 ArgInCurrentTimeInt64;

struct ReturnCurrentTimeInt64 {
    sqlite3_int64 piNow;
    int rc;
};
typedef struct ReturnCurrentTimeInt64 ReturnCurrentTimeInt64;

void unixCurrentTimeInt64ConvertArgInToChar(sqlite3_vfs *NotUsed, sqlite3_int64 *piNow, char *arg) {
    ArgInCurrentTimeInt64 *arg_struct = (ArgInCurrentTimeInt64 *) arg;
    arg_struct->piNow = *piNow;
}
void unixCurrentTimeInt64ConvertCharToReturn(const char *arg, sqlite3_int64 *piNow, int *pRc) {
    ReturnCurrentTimeInt64 *arg_struct = (ReturnCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
    memcpy(pRc, &arg_struct->rc, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertCharToArgIn(const char *arg, sqlite3_vfs *NotUsed, sqlite3_int64 *piNow) {
    ArgInCurrentTimeInt64 *arg_struct = (ArgInCurrentTimeInt64 *) arg;
    memcpy(piNow, &arg_struct->piNow, sizeof(sqlite3_int64));
}
void unixCurrentTimeInt64ConvertReturnToChar(sqlite3_int64 *piNow, int *pRc, char *arg) {
    ReturnCurrentTimeInt64 *arg_struct = (ReturnCurrentTimeInt64 *) arg;
    memcpy(&arg_struct->piNow, piNow, sizeof(sqlite3_int64));
    memcpy(&arg_struct->rc, pRc, sizeof(int));
}
*/


/////////////// function unixGetLastError //////////////////////////////////////////
//static int unixGetLastError(sqlite3_vfs *NotUsed, int NotUsed2, char *NotUsed3){
struct ArgInGetLastError {
    int NotUsed2;
    char NotUsed3[2];
};
typedef struct ArgInGetLastError ArgInGetLastError;
struct ReturnGetLastError {
    int rc_errno;
};
typedef struct ReturnGetLastError ReturnGetLastError;

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
struct ArgInRead {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[MAX_SIZE];
    int amt;
    sqlite3_int64 offset;
};
typedef struct ArgInRead ArgInRead;

struct ReturnRead {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[MAX_SIZE];
    int rc;
};
typedef struct ReturnRead ReturnRead;

void unixReadConvertArgInToChar(    // client
        sqlite3_file *id,
        char *pBuf,
        int amt,
        sqlite3_int64 offset,
        char *arg
) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pBuf, amt);
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE, &amt, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE + sizeof(int), &offset, sizeof(sqlite3_int64));
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
    memcpy(pRc, arg + SIZE_UNIXFILE + MAX_SIZE, sizeof(int));
}

void unixReadConvertCharToArgIn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        sqlite3_int64 *offset
) {

    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(amt, arg + SIZE_UNIXFILE + MAX_SIZE, sizeof(int));
    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(offset, arg + SIZE_UNIXFILE + MAX_SIZE + sizeof(int), sizeof(sqlite3_int64));
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
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE, pRc, sizeof(int));
}


/////////////// function unixWrite  //////////////////////////////////////////
struct ArgInWrite {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[MAX_SIZE];
    int amt;
    sqlite3_int64 offset;
};
typedef struct ArgInWrite ArgInWrite;

struct ReturnWrite {
    char file_infor[SIZE_UNIXFILE];
    char pBuf[MAX_SIZE];
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
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE, &amt, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE + sizeof(int), &offset, sizeof(sqlite3_int64));
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
    memcpy(pRc, arg + SIZE_UNIXFILE + MAX_SIZE, sizeof(int));
}

void unixWriteConvertCharToArgIn(
        const char *arg,
        sqlite3_file *id,
        char *pBuf,
        int *amt,
        sqlite3_int64 *offset
) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(amt, arg + SIZE_UNIXFILE + MAX_SIZE, sizeof(int));
    memcpy(pBuf, arg + SIZE_UNIXFILE, *amt);
    memcpy(offset, arg + SIZE_UNIXFILE + MAX_SIZE + sizeof(int), sizeof(sqlite3_int64));
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
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE, pRc, sizeof(int));
}

/////////////// function unixTruncate  //////////////////////////////////////////
struct ArgInTruncate {
    char file_infor[SIZE_UNIXFILE];
    i64 nByte;
};
typedef struct ArgInTruncate ArgInTruncate;

struct ReturnTruncate {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnTruncate ReturnTruncate;

void unixTruncateConvertArgInToChar(sqlite3_file *id, i64 nByte, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &nByte, sizeof(i64));
}

void unixTruncateConvertCharToReturn(const char *arg, sqlite3_file *id, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pRc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixTruncateConvertCharToArgIn(const char *arg, sqlite3_file *id, i64 *nByte) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(nByte, arg + SIZE_UNIXFILE, sizeof(i64));
}

void unixTruncateConvertReturnToChar(sqlite3_file *id, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pRc, sizeof(int));
}

/////////////// function unixSync  //////////////////////////////////////////
struct ArgInSync {
    char file_infor[SIZE_UNIXFILE];
    int flags;
};
typedef struct ArgInSync ArgInSync;

struct ReturnSync {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnSync ReturnSync;

void unixSyncConvertArgInToChar(sqlite3_file *id, int flags, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &flags, sizeof(int));
}

void unixSyncConvertCharToReturn(const char *arg, sqlite3_file *id, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pRc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixSyncConvertCharToArgIn(const char *arg, sqlite3_file *id, int *flags) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(flags, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixSyncConvertReturnToChar(sqlite3_file *id, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pRc, sizeof(int));
}

/////////////// function unixFileSize  //////////////////////////////////////////
struct ArgInFileSize {
    char file_infor[SIZE_UNIXFILE];
    i64 pSize;
};
typedef struct ArgInFileSize ArgInFileSize;

struct ReturnFileSize {
    char file_infor[SIZE_UNIXFILE];
    i64 pSize;
    int rc;
};
typedef struct ReturnFileSize ReturnFileSize;

void unixFileSizeConvertArgInToChar(sqlite3_file *id, i64 *pSize, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
//    memcpy(arg + SIZE_UNIXFILE, pSize, sizeof(i64));
}

void unixFileSizeConvertCharToReturn(const char *arg, sqlite3_file *id, i64 *pSize, int *rc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pSize, arg + SIZE_UNIXFILE, sizeof(i64));
    memcpy(rc, arg + SIZE_UNIXFILE + sizeof(i64), sizeof(int));
}

void unixFileSizeConvertCharToArgIn(const char *arg, sqlite3_file *id, i64 *pSize) {
    memcpy(id, arg, SIZE_UNIXFILE);
//    memcpy(pSize, arg + SIZE_UNIXFILE, sizeof(i64));
}

void unixFileSizeConvertReturnToChar(sqlite3_file *id, i64 *pSize, int *rc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pSize, sizeof(i64));
    memcpy(arg + SIZE_UNIXFILE + sizeof(i64), rc, sizeof(int));
}

/////////////// function unixFileControl  //////////////////////////////////////////
struct ArgInFileControl {
    char file_infor[SIZE_UNIXFILE];
    int op;
    int pArg;
};
typedef struct ArgInFileControl ArgInFileControl;
struct ReturnFileControl {
    char file_infor[SIZE_UNIXFILE];
    int pArg;
    int rc;
};
typedef struct ReturnFileControl ReturnFileControl;

void unixFileControlConvertArgInToChar(sqlite3_file *id, int op, void *pArg, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &op, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + sizeof(int), pArg, sizeof(int));
}

void unixFileControlConvertCharToReturn(const char *arg, sqlite3_file *id, int *pArg, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pArg, arg + SIZE_UNIXFILE, sizeof(int));
    memcpy(pRc, arg + SIZE_UNIXFILE + sizeof(int), sizeof(int));
}

void unixFileControlConvertCharToArgIn(const char *arg, sqlite3_file *id, int *op, void *pArg) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(op, arg + SIZE_UNIXFILE, sizeof(int));
    memcpy(pArg, arg + SIZE_UNIXFILE + sizeof(int), sizeof(int));
}

void unixFileControlConvertReturnToChar(sqlite3_file *id, int *pArg, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pArg, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + sizeof(int), pRc, sizeof(int));
}

/////////////// function unixSectorSize  //////////////////////////////////////////
//SectorSize
struct ArgInSectorSize {
    char file_infor[SIZE_UNIXFILE];
};
typedef struct ArgInSectorSize ArgInSectorSize;

struct ReturnSectorSize {
    char file_infor[SIZE_UNIXFILE];
    int sectorSize;
};
typedef struct ReturnSectorSize ReturnSectorSize;

void unixSectorSizeConvertArgInToChar(sqlite3_file *id, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
}

void unixSectorSizeConvertCharToReturn(const char *arg, sqlite3_file *id, int *sectorSize) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(sectorSize, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixSectorSizeConvertCharToArgIn(const char *arg, sqlite3_file *id) {
    memcpy(id, arg, SIZE_UNIXFILE);
}

void unixSectorSizeConvertReturnToChar(sqlite3_file *id, int *sectorSize, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, sectorSize, sizeof(int));
}

/////////////// function unixDeviceCharacteristics  //////////////////////////////////////////
struct ArgInDeviceCharacteristics {
    char file_infor[SIZE_UNIXFILE];
};
typedef struct ArgInDeviceCharacteristics ArgInDeviceCharacteristics;

struct ReturnDeviceCharacteristics {
    char file_infor[SIZE_UNIXFILE];
    int deviceCharacteristics;
};
typedef struct ReturnDeviceCharacteristics ReturnDeviceCharacteristics;

void unixDeviceCharacteristicsConvertArgInToChar(sqlite3_file *id, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
}

void unixDeviceCharacteristicsConvertCharToReturn(const char *arg, sqlite3_file *id, int *deviceCharacteristics) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(deviceCharacteristics, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixDeviceCharacteristicsConvertCharToArgIn(const char *arg, sqlite3_file *id) {
    memcpy(id, arg, SIZE_UNIXFILE);
}

void unixDeviceCharacteristicsConvertReturnToChar(sqlite3_file *id, int *deviceCharacteristics, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, deviceCharacteristics, sizeof(int));
}

/////////////// function unixClose  //////////////////////////////////////////
struct ArgInClose {
    char file_infor[SIZE_UNIXFILE];
};
typedef struct ArgInClose ArgInClose;

struct ReturnClose {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnClose ReturnClose;

void unixCloseConvertArgInToChar(sqlite3_file *id, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
}

void unixCloseConvertCharToReturn(const char *arg, sqlite3_file *id, int *rc) {
//    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(rc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixCloseConvertCharToArgIn(const char *arg, sqlite3_file *id) {
    memcpy(id, arg, SIZE_UNIXFILE);
}

void unixCloseConvertReturnToChar(sqlite3_file *id, int *rc, char *arg) {
//    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, rc, sizeof(int));
}

/////////////// function unixLock  //////////////////////////////////////////
struct ArgInLock {
    char file_infor[SIZE_UNIXFILE];
    int eFileLock;
};
typedef struct ArgInLock ArgInLock;

struct ReturnLock {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnLock ReturnLock;

void unixLockConvertArgInToChar(sqlite3_file *id, int eFileLock, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &eFileLock, sizeof(int));
}

void unixLockConvertCharToReturn(const char *arg, sqlite3_file *id, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pRc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixLockConvertCharToArgIn(const char *arg, sqlite3_file *id, int *eFileLock) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(eFileLock, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixLockConvertReturnToChar(sqlite3_file *id, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pRc, sizeof(int));
}

/////////////// function unixUnlock  //////////////////////////////////////////
struct ArgInUnlock {
    char file_infor[SIZE_UNIXFILE];
    int eFileLock;
};
typedef struct ArgInUnlock ArgInUnlock;

struct ReturnUnlock {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnUnlock ReturnUnlock;

void unixUnlockConvertArgInToChar(sqlite3_file *id, int eFileLock, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &eFileLock, sizeof(int));
}

void unixUnlockConvertCharToReturn(const char *arg, sqlite3_file *id, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pRc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixUnlockConvertCharToArgIn(const char *arg, sqlite3_file *id, int *eFileLock) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(eFileLock, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixUnlockConvertReturnToChar(sqlite3_file *id, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pRc, sizeof(int));
}

/////////////// function unixCheckReservedLock  //////////////////////////////////////////
//static int unixCheckReservedLock(sqlite3_file *id, int *pResOut
struct ArgInCheckReservedLock {
    char file_infor[SIZE_UNIXFILE];
    int pResOut;
};
typedef struct ArgInCheckReservedLock ArgInCheckReservedLock;

struct ReturnCheckReservedLock {
    char file_infor[SIZE_UNIXFILE];
    int pResOut;
    int rc;
};
typedef struct ReturnCheckReservedLock ReturnCheckReservedLock;

void unixCheckReservedLockConvertArgInToChar(sqlite3_file *id, int *pResOut, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pResOut, sizeof(int));
}

void unixCheckReservedLockConvertCharToReturn(const char *arg, sqlite3_file *id, int *pResOut, int *pRc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pResOut, arg + SIZE_UNIXFILE, sizeof(int));
    memcpy(pRc, arg + SIZE_UNIXFILE + sizeof(int), sizeof(int));
}

void unixCheckReservedLockConvertCharToArgIn(const char *arg, sqlite3_file *id, int *pResOut) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pResOut, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixCheckReservedLockConvertReturnToChar(sqlite3_file *id, int *pResOut, int *pRc, char *arg) {
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pResOut, sizeof(int));
    memcpy(arg + SIZE_UNIXFILE + sizeof(int), pRc, sizeof(int));
}

/////////////// function unixFetch  //////////////////////////////////////////
struct ArgInFetch {
    char file_infor[SIZE_UNIXFILE];
    i64 ioff;
    int nAmt;
    char pData[MAX_SIZE];
};
typedef struct ArgInFetch ArgInFetch;

struct ReturnFetch {
    char file_infor[SIZE_UNIXFILE];
    char pData[MAX_SIZE];
    int rc;
};

void unixFetchConvertArgInToChar(sqlite3_file *fd, i64 iOff, int nAmt, void **pp, char *arg) {
    memcpy(arg, fd, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &iOff, sizeof(i64));
    memcpy(arg + SIZE_UNIXFILE + sizeof(i64), &nAmt, sizeof(int));
//    memcpy(arg+SIZE_UNIXFILE+ sizeof(i64)+ sizeof(int), *pp, nAmt); // pp
}

void unixFetchConvertCharToReturn(const char *arg, sqlite3_file *fd, char *pp, int nAmt, int *rc) {
    memcpy(fd, arg, SIZE_UNIXFILE);
    memcpy(pp, arg + SIZE_UNIXFILE, nAmt);
    memcpy(rc, arg + SIZE_UNIXFILE + MAX_SIZE, sizeof(int));
}

void unixFetchConvertCharToArgIn(const char *arg, sqlite3_file *fd, i64 *iOff, int *nAmt, void **pp) {
    memcpy(fd, arg, SIZE_UNIXFILE);
    memcpy(iOff, arg + SIZE_UNIXFILE, sizeof(i64));
    memcpy(nAmt, arg + SIZE_UNIXFILE + sizeof(i64), sizeof(int));
//    memcpy();
}

void unixFetchsConvertReturnToChar(sqlite3_file *fd, char *pp, int nAmt, int *rc, char *arg) {
    memcpy(arg, fd, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, pp, nAmt);
    memcpy(arg + SIZE_UNIXFILE + MAX_SIZE, rc, sizeof(int));
}

/////////////// function unixUnfetch  //////////////////////////////////////////
struct ArgInUnfetch {
    char file_infor[SIZE_UNIXFILE];
    i64 ioff;
    int p_flag;
};
typedef struct ArgInUnfetch ArgInUnfetch;

struct ReturnUnfetch {
    char file_infor[SIZE_UNIXFILE];
    int rc;
};
typedef struct ReturnUnfetch ReturnUnfetch;

void unixUnfetchConvertArgInToChar(sqlite3_file *fd, i64 iOff, int *p_flag, char *arg) {
    memcpy(arg, fd, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &iOff, sizeof(i64));
    memcpy(arg + SIZE_UNIXFILE + sizeof(i64), p_flag, sizeof(int));
}

void unixUnfetchConvertCharToReturn(const char *arg, sqlite3_file *fd, int *rc) {
    memcpy(fd, arg, sizeof(SIZE_UNIXFILE));
    memcpy(rc, arg + SIZE_UNIXFILE, sizeof(int));
}

void unixUnfetchConvertCharToArgIn(const char *arg, sqlite3_file *fd, i64 *iOff, int *p_flag) {
    memcpy(fd, arg, SIZE_UNIXFILE);
    memcpy(iOff, arg + SIZE_UNIXFILE, sizeof(i64));
    memcpy(p_flag, arg + SIZE_UNIXFILE + sizeof(i64), sizeof(int));
}

void unixUnfetchConvertReturnToChar(sqlite3_file *fd, int *rc, char *arg) {
    memcpy(arg, fd, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, rc, sizeof(int));
}

//void nameConvertArgInToChar(){}
//void nameConvertCharToReturn(){}
//void nameConvertCharToArgIn(){}
//void nameConvertReturnToChar(){}