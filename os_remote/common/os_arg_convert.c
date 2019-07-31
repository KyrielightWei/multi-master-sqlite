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

#define OS_REMOTE_DEBUG 0

#if  OS_REMOTE_DEBUG
#include "common_sqlite3.c"
#endif

#define SIZE_UNIXFILE sizeof(unixFile)
#define MAX_SIZE 8192

/////////////// MethodAndFD ////////////////////////////////////////////
struct MethodAndFD {
    int fd;
    sqlite3_io_methods *method;
    char path[512];
    struct MethodAndFD *next;
};
typedef struct MethodAndFD MethodAndFD;

MethodAndFD *listHead = NULL;

void InitList() {
    listHead = (MethodAndFD *) malloc(sizeof(MethodAndFD));
    listHead->next = NULL;
}

void PrintList() {
    printf("   ---PrintList:\n");
    if (NULL == listHead) { InitList(); }
    MethodAndFD *temp = listHead->next;
    while (NULL != temp) {
        printf("      fd : %d, method : %d\n", temp->fd, temp->method);
        temp = temp->next;
    }
}

MethodAndFD *FindNode(int fd, MethodAndFD **pre) {
    if (NULL == listHead) { InitList(); }
    MethodAndFD *temp = listHead->next;
    while (temp != NULL) {
        if (fd == temp->fd) {
            return temp;
        }
        *pre = temp;
        temp = temp->next;
    }
    return temp;
}

void AddNode(int fd, sqlite3_io_methods *method) {
    if (NULL == listHead) { InitList(); }
    MethodAndFD *prev = listHead;
    MethodAndFD *node = FindNode(fd, &prev);

    if (NULL == node) {
        MethodAndFD *temp = (MethodAndFD *) malloc(sizeof(MethodAndFD));
        temp->fd = fd;
        temp->method = method;
        temp->next = node;
        prev->next = temp;
    } else {
        node->method = method;
    }
}

void DeleteNode(int fd) {
    if (NULL == listHead) { InitList(); }
    MethodAndFD *prev = listHead;
    MethodAndFD *node = FindNode(fd, &prev);
    if (NULL != node) {
        prev->next = prev->next->next;
        free(node);
    }
}
/////////////// end MethodAndFD ////////////////////////////////////////////

/////////////// client and server PMethods ////////////////////////////////////////////
extern void setClientRemotePMethods(sqlite3_file *pf);

void setServicePath(int fd, const char *path) {
    MethodAndFD *prev = listHead;
    MethodAndFD *node = FindNode(fd, &prev);
    if (NULL != node) {
//        TODO : when use memset , program will wrong
//        memset(node->path, 0, 512);
        memcpy(node->path, path, strlen(path));
    }
}

void getServicePath(int fd, unixFile *id) {
    MethodAndFD *prev = listHead;
    MethodAndFD *node = FindNode(fd, &prev);
    if (NULL != node) {
        id->zPath = node->path;
    }
}

void setServerUnixPMethods(int fd, sqlite3_file *pf) {
    AddNode(fd, (sqlite3_io_methods *) pf->pMethods);
}

void getServerUnixPMethods(int fd, sqlite3_file *pf) {
    MethodAndFD *prev = listHead;
    MethodAndFD *node = FindNode(fd, &prev);
    if (NULL == node || NULL == node->method) {
        pf->pMethods = &posixIoMethods;
    } else {
        pf->pMethods = node->method;
    }

//    memset(path, 0, 512);
//    memcpy(path, node->path, strlen(node->path));
}
/////////////// end PMethods ////////////////////////////////////////////



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

/*
 * type :
 * 1 : int
 * 2 : i64
 * 3 : char
 * 0 : other
 */
int ControlType(int op) {
    if (SQLITE_FCNTL_LOCKSTATE == op || SQLITE_FCNTL_LAST_ERRNO == op || SQLITE_FCNTL_CHUNK_SIZE == op ||
        SQLITE_FCNTL_PERSIST_WAL == op || SQLITE_FCNTL_POWERSAFE_OVERWRITE == op || SQLITE_FCNTL_HAS_MOVED == op ||
        SQLITE_FCNTL_LOCK_TIMEOUT == op) {
        return 1;
    } else if (SQLITE_FCNTL_SIZE_HINT == op || SQLITE_FCNTL_MMAP_SIZE == op
               || SQLITE_FCNTL_PDB == op || SQLITE_FCNTL_BUSYHANDLER == op || SQLITE_FCNTL_CHUNK_SIZE == op ||
               SQLITE_FCNTL_PERSIST_WAL == op
            ) {
        return 2;
    } else if (SQLITE_FCNTL_VFSNAME == op || SQLITE_FCNTL_TEMPFILENAME == op || SQLITE_FCNTL_GET_LOCKPROXYFILE == op) {
        return 3;
    }
    return 0;
}

int LenFileControlPArg(int op, void *pArg) {
    if (pArg == NULL || pArg == 0 || (char *) pArg == NULL) {
        return 0;
    }
    int type = ControlType(op);
    if (1 == type) {
//        printf("      control type = %s\n", "int");
        return sizeof(int);
    } else if (2 == type) {
//        printf("      control type = %s\n", "i64");
        return sizeof(i64);
    } else if (3 == type) {
//        printf("      control type = %s\n", "char");
        return strlen(*(char **) pArg);
    }
//    printf("      control type = %s\n", "other");
    return 0;
}


/////////////// sqlite3_os_init ////////////////////////////////////////////
struct ArgInInit {

};
typedef struct ArgInInit ArgInInit;

struct ReturnInit {
    int rc;
};
typedef struct ReturnInit ReturnInit;

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

    int f = -1;
    if (!pOutFlags) {
        pOutFlags = &f;
    }
    memcpy(arg + 512 + SIZE_UNIXFILE + sizeof(int), pOutFlags, sizeof(int));
}

void unixOpenConvertCharToReturn(         // Client
        const char *arg,
        sqlite3_file *pFile,
        int *pOutFlags,
        int *pRc
) {
    memcpy(pFile, arg, SIZE_UNIXFILE);
    int temp = 0;
    memcpy(&temp, arg + SIZE_UNIXFILE, sizeof(int));
    memcpy(pRc, arg + SIZE_UNIXFILE + sizeof(int), sizeof(int));

    if (-1 == temp) {
        // pOutFlags = 0;
    } else {
        memcpy(pOutFlags, arg + SIZE_UNIXFILE, sizeof(int));
    }

    setClientRemotePMethods(pFile);
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
    // printf("pOutflags2 = %d\n", *pOutFlags);
    memcpy(arg + SIZE_UNIXFILE + sizeof(int), pRc, sizeof(int));

//    printf("%d\n", *(int *)(arg + SIZE_UNIXFILE));
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

// //#define GetArgInFullPathname(OUT_Len) 
// struct ArgInFullPathname {   
//     int nOut;                      
//     char * path;            
//     //char zOut[OUT_Len];           
// };                           
// typedef struct ArgInFullPathname ArgInFullPathname; 
// struct ReturnFullPathname {  
//     char zOut[OUT_Len];          
//     int rc;                  
// };                           
// typedef struct ReturnFullPathname ReturnFullPathname;

int getFullPathname_ARG_LEN(int nOut) {
    return sizeof(int) + nOut;
}

int getFullPathname_RETURN_LEN(int nOut) {
    return sizeof(int) + nOut;
}

int getFullPathname_OUT_LEN(const char *arg) {
    return *((const int *) arg);
}

void unixFullPathnameConvertArgInToChar(// client
        sqlite3_vfs *pVfs,
        const char *zPath,
        int nOut,
        char *zOut,
        char *arg
) {
    memcpy(arg, &nOut, sizeof(int));
    memcpy(arg + sizeof(int), zPath, nOut);

//    memcpy(arg + 512 + sizeof(int), zOut, 512);
}

void unixFullPathnameConvertCharToReturn( // client
        const char *arg,
        int nOut,
        char *zOut,
        int *pRc
) {
    //strcpy(zOut, arg);
    memcpy(zOut, arg, nOut);
    memcpy(pRc, arg + nOut, sizeof(int));
    //printf("CONVERT CHAR TO RETURN  Zout = %s\n",zOut);
}

void unixFullPathnameConvertCharToArgIn( // server
        const char *arg,
        sqlite3_vfs *pVfs,
        char *zPath,
        int *nOut,
        char *zOut
) {
    *nOut = getFullPathname_OUT_LEN(arg);
    strcpy(zPath, arg + sizeof(int));
    //memcpy(nOut, arg + 512, sizeof(int));
//    strcpy(zOut, arg + 512 + sizeof(int));
}

void unixFullPathnameConvertReturnToChar( //server
        int nOut,
        char *zOut,
        int *pRc,
        char *arg
) {
    printf("CONVERT RETURN TO CHAR Zout = %s\n", zOut);
    strcpy(arg, zOut);
    memcpy(arg + nOut, pRc, sizeof(int));
    printf("CONVERT RETURN TO CHAR char = %s\n", (char *) arg);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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
    memcpy(arg + SIZE_UNIXFILE, pSize, sizeof(i64));
}

void unixFileSizeConvertCharToReturn(const char *arg, sqlite3_file *id, i64 *pSize, int *rc) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pSize, arg + SIZE_UNIXFILE, sizeof(i64));
    memcpy(rc, arg + SIZE_UNIXFILE + sizeof(i64), sizeof(int));

    setClientRemotePMethods(id);
}

void unixFileSizeConvertCharToArgIn(const char *arg, sqlite3_file *id, i64 *pSize) {
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(pSize, arg + SIZE_UNIXFILE, sizeof(i64));
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
    char pArg[512];
    int size;
};
typedef struct ArgInFileControl ArgInFileControl;
struct ReturnFileControl {
    char file_infor[SIZE_UNIXFILE];
    char pArg[512];
    int rc;
    int size;
};
typedef struct ReturnFileControl ReturnFileControl;

void unixFileControlConvertArgInToChar(sqlite3_file *id, int op, void *pArg, char *arg, int *size) {   //  client
    memcpy(arg, id, SIZE_UNIXFILE);
    memcpy(arg + SIZE_UNIXFILE, &op, sizeof(int));
    if (*size > 0) {
        memcpy(arg + SIZE_UNIXFILE + sizeof(int), pArg, *size);
    }
    memcpy(arg + SIZE_UNIXFILE + sizeof(int) + 512, size, sizeof(int));
}

void unixFileControlConvertCharToReturn(const char *arg, sqlite3_file *id, char *pArg, int *pRc, int op) {   //  client
    int size;
    memcpy(&size, arg + SIZE_UNIXFILE + 512 + sizeof(int), sizeof(int));
    memcpy(id, arg, SIZE_UNIXFILE);
    if (size > 0) {
        memcpy(pArg, arg + SIZE_UNIXFILE, size);
    }
    memcpy(pRc, arg + SIZE_UNIXFILE + 512, sizeof(int));

    setClientRemotePMethods(id);
}

void
unixFileControlConvertCharToArgIn(const char *arg, sqlite3_file *id, int *op, void *pArg, int *size) {    //  server
    memcpy(size, arg + SIZE_UNIXFILE + sizeof(int) + 512, sizeof(int));
    memcpy(id, arg, SIZE_UNIXFILE);
    memcpy(op, arg + SIZE_UNIXFILE, sizeof(int));
    if (*size > 0) {
        memcpy(pArg, arg + SIZE_UNIXFILE + sizeof(int), *size);
    }
}

void
unixFileControlConvertReturnToChar(sqlite3_file *id, char *pArg, int *pRc, char *arg, int op, int *size) {    //  server
    memcpy(arg + SIZE_UNIXFILE + 512 + sizeof(int), size, sizeof(int));
    memcpy(arg, id, SIZE_UNIXFILE);
    if (*size > 0) {
        memcpy(arg + SIZE_UNIXFILE, pArg, *size);
    }
    memcpy(arg + SIZE_UNIXFILE + 512, pRc, sizeof(int));

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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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

    setClientRemotePMethods(id);
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
typedef struct ReturnFetch ReturnFetch;

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

    setClientRemotePMethods(fd);
}

void unixFetchConvertCharToArgIn(const char *arg, sqlite3_file *fd, i64 *iOff, int *nAmt, void **pp) {
    memcpy(fd, arg, SIZE_UNIXFILE);
    memcpy(iOff, arg + SIZE_UNIXFILE, sizeof(i64));
    memcpy(nAmt, arg + SIZE_UNIXFILE + sizeof(i64), sizeof(int));
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

    setClientRemotePMethods(fd);
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