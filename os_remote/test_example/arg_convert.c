#include "sqlite3.c"

typedef unsigned int u32;


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
 * 结构体声明  命名：Arg函数名
 * 在服务端的server_wrap中需要复制，定义结构体，只有在包含sqlite3.c的文件中才能定义这些结构体，因此无法放于头文件复用
 */
struct   ArgInUnixOpen
{
  //vfs忽略
  char path[512];
  char file_infor[sizeof(unixFile)];
  int in_flags;
  int out_flags;
};
typedef struct  ArgInUnixOpen ArgInUnixOpen;

struct   ArgOutUnixOpen
{
  char file_infor[sizeof(unixFile)];
  int out_flags;
  int rc;
};
typedef struct  ArgOutUnixOpen ArgOutUnixOpen;


/**
 * 转换函数 命名：函数名_convertArgToChar(函数参数,char *) //转换前预先分配结构体占用的char数组
 *               函数名_convertCharToArg(char * ,函数参数) //转换前预先分配函数参数占用的空间，传入指针
 */
void convertUnixOpenToCharIn(sqlite3_vfs *pVfs,        //Client   
  const char *zPath,           
  sqlite3_file *pFile,         
  int flags,                   
  int *pOutFlags,char * arg)
{
  ArgInUnixOpen * arg_struct = (ArgInUnixOpen *)arg;
  strcpy(arg_struct->path,zPath);
  memcpy(arg_struct->file_infor,pFile,sizeof(unixFile));
  //memcpy(&arg_struct->in_flags,&flags,sizeof(int));
  //memcpy(&arg_struct->out_flags,pOutFlags,sizeof(int));
  arg_struct->in_flags = flags;
  arg_struct->out_flags = *pOutFlags;
  //printf("%d\n",flags);
   //printf("%d\n",*pOutFlags);
  return;
}

void convertCharOutToUnixOpen(const char * arg,          // Client
  sqlite3_file *pFile,
  int *pOutFlags,
  int * pRc 
  )
{
    ArgOutUnixOpen * arg_struct = (ArgOutUnixOpen *)arg;
    memcpy(pFile,arg_struct->file_infor,sizeof(unixFile));
    memcpy(pOutFlags,&arg_struct->out_flags,sizeof(int));
    memcpy(pRc,&arg_struct->rc,sizeof(int));

    return;
}

void convertCharInToUnixOpen(sqlite3_vfs *pVfs,        //server  
  char *zPath,           
  sqlite3_file *pFile,         
  int * flags,                   
  int *pOutFlags,const char * arg)
{
  ArgInUnixOpen * arg_struct = (ArgInUnixOpen *)arg;
  strcpy(zPath,arg_struct->path);
  memcpy(pFile,arg_struct->file_infor,sizeof(unixFile));
  memcpy(flags,&arg_struct->in_flags,sizeof(int));
  memcpy(pOutFlags,&arg_struct->out_flags,sizeof(int));

  return;
}

void convertUnixOpenToCharOut(char * arg,          // server
  sqlite3_file *pFile,
  int *pOutFlags,
  int * pRc 
  )
{
  ArgOutUnixOpen * arg_struct = (ArgOutUnixOpen *)arg;
  memcpy(arg_struct->file_infor,pFile,sizeof(unixFile));
  memcpy(&arg_struct->out_flags,pOutFlags,sizeof(int));
  memcpy(&arg_struct->rc,pRc,sizeof(int));

  return;
}