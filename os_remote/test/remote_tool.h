/**
 * File : some tool function for rpc
 */
#ifndef  REMOTE_TOOL

#include "sqlite3.c"
#include <string>

struct remote_file
{
  //##### unixInodeInfo *pInode;              /* Info about locks on this inode */
  /**
   * VFS 不论,客户端指定为remote_vfs,服务端指定一种本地VFS,默认传递和返回
   * //#####表示todo
   * //@ 表示需要转换
   */
  int h;                              /* The file descriptor */
  unsigned char eFileLock;            /* The type of lock held on this fd */
  unsigned short int ctrlFlags;       /* Behavioral bits.  UNIXFILE_* flags */
  int lastErrno;                      /* The unix errno from last I/O error */
  //##### void *lockingContext;               /* Locking style specific state */
  //##### UnixUnusedFd *pPreallocatedUnused;  /* Pre-allocated UnixUnusedFd */
  std::string zPath; //@                 /* Name of the file */
  //##### unixShm *pShm;                      /* Shared memory segment information */
  int szChunk;                        /* Configured by FCNTL_CHUNK_SIZE */

  int sectorSize;                     /* Device sector size */
  int deviceCharacteristics;          /* Precomputed device characteristics */
};

typedef unsigned int u32;

void ConvertStringToBytes(const std::string & mess,char * bytes,u32 bytes_len);

void ConvertBytesToString(const char * bytes,std::string & mess,u32 bytes_len);




#endif // ! REMOTE_TOOL