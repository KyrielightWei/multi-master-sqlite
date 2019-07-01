#ifndef STORAGE_SERVER

/*
** standard include files.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#if !defined(SQLITE_OMIT_WAL) || SQLITE_MAX_MMAP_SIZE>0
# include <sys/mman.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>


//#include<sqlite3.h>

/*
** CAPI3REF: Result Codes
** KEYWORDS: {result code definitions}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicate success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [extended result code definitions]
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* Generic error */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Internal use only */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Not used */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */


typedef long long int sqlite3_int64;
typedef sqlite_int64 i64;   


/*
** Many system calls are accessed through pointer-to-functions so that
** they may be overridden at runtime to facilitate fault injection during
** testing and sandboxing.  The following array holds the names and pointers
** to all overrideable system calls.
*/
static struct unix_syscall {
  const char *zName;            /* Name of the system call */
  sqlite3_syscall_ptr pCurrent; /* Current value of the system call */
  sqlite3_syscall_ptr pDefault; /* Default value */
} aSyscall[] = {
  { "open",         (sqlite3_syscall_ptr)posixOpen,  0  },
#define osOpen      ((int(*)(const char*,int,int))aSyscall[0].pCurrent)

  { "close",        (sqlite3_syscall_ptr)close,      0  },
#define osClose     ((int(*)(int))aSyscall[1].pCurrent)

  { "access",       (sqlite3_syscall_ptr)access,     0  },
#define osAccess    ((int(*)(const char*,int))aSyscall[2].pCurrent)

  { "getcwd",       (sqlite3_syscall_ptr)getcwd,     0  },
#define osGetcwd    ((char*(*)(char*,size_t))aSyscall[3].pCurrent)

  { "stat",         (sqlite3_syscall_ptr)stat,       0  },
#define osStat      ((int(*)(const char*,struct stat*))aSyscall[4].pCurrent)

/*
** The DJGPP compiler environment looks mostly like Unix, but it
** lacks the fcntl() system call.  So redefine fcntl() to be something
** that always succeeds.  This means that locking does not occur under
** DJGPP.  But it is DOS - what did you expect?
*/
#ifdef __DJGPP__
  { "fstat",        0,                 0  },
#define osFstat(a,b,c)    0
#else     
  { "fstat",        (sqlite3_syscall_ptr)fstat,      0  },
#define osFstat     ((int(*)(int,struct stat*))aSyscall[5].pCurrent)
#endif

  { "ftruncate",    (sqlite3_syscall_ptr)ftruncate,  0  },
#define osFtruncate ((int(*)(int,off_t))aSyscall[6].pCurrent)

  { "fcntl",        (sqlite3_syscall_ptr)fcntl,      0  },
#define osFcntl     ((int(*)(int,int,...))aSyscall[7].pCurrent)

  { "read",         (sqlite3_syscall_ptr)read,       0  },
#define osRead      ((ssize_t(*)(int,void*,size_t))aSyscall[8].pCurrent)

#if defined(USE_PREAD) || SQLITE_ENABLE_LOCKING_STYLE
  { "pread",        (sqlite3_syscall_ptr)pread,      0  },
#else
  { "pread",        (sqlite3_syscall_ptr)0,          0  },
#endif
#define osPread     ((ssize_t(*)(int,void*,size_t,off_t))aSyscall[9].pCurrent)

#if defined(USE_PREAD64)
  { "pread64",      (sqlite3_syscall_ptr)pread64,    0  },
#else
  { "pread64",      (sqlite3_syscall_ptr)0,          0  },
#endif
#define osPread64 ((ssize_t(*)(int,void*,size_t,off64_t))aSyscall[10].pCurrent)

  { "write",        (sqlite3_syscall_ptr)write,      0  },
#define osWrite     ((ssize_t(*)(int,const void*,size_t))aSyscall[11].pCurrent)

#if defined(USE_PREAD) || SQLITE_ENABLE_LOCKING_STYLE
  { "pwrite",       (sqlite3_syscall_ptr)pwrite,     0  },
#else
  { "pwrite",       (sqlite3_syscall_ptr)0,          0  },
#endif
#define osPwrite    ((ssize_t(*)(int,const void*,size_t,off_t))\
                    aSyscall[12].pCurrent)

#if defined(USE_PREAD64)
  { "pwrite64",     (sqlite3_syscall_ptr)pwrite64,   0  },
#else
  { "pwrite64",     (sqlite3_syscall_ptr)0,          0  },
#endif
#define osPwrite64  ((ssize_t(*)(int,const void*,size_t,off64_t))\
                    aSyscall[13].pCurrent)

  { "fchmod",       (sqlite3_syscall_ptr)fchmod,          0  },
#define osFchmod    ((int(*)(int,mode_t))aSyscall[14].pCurrent)

#if defined(HAVE_POSIX_FALLOCATE) && HAVE_POSIX_FALLOCATE
  { "fallocate",    (sqlite3_syscall_ptr)posix_fallocate,  0 },
#else
  { "fallocate",    (sqlite3_syscall_ptr)0,                0 },
#endif
#define osFallocate ((int(*)(int,off_t,off_t))aSyscall[15].pCurrent)

  { "unlink",       (sqlite3_syscall_ptr)unlink,           0 },
#define osUnlink    ((int(*)(const char*))aSyscall[16].pCurrent)

  { "openDirectory",    (sqlite3_syscall_ptr)openDirectory,      0 },
#define osOpenDirectory ((int(*)(const char*,int*))aSyscall[17].pCurrent)

  { "mkdir",        (sqlite3_syscall_ptr)mkdir,           0 },
#define osMkdir     ((int(*)(const char*,mode_t))aSyscall[18].pCurrent)

  { "rmdir",        (sqlite3_syscall_ptr)rmdir,           0 },
#define osRmdir     ((int(*)(const char*))aSyscall[19].pCurrent)

#if defined(HAVE_FCHOWN)
  { "fchown",       (sqlite3_syscall_ptr)fchown,          0 },
#else
  { "fchown",       (sqlite3_syscall_ptr)0,               0 },
#endif
#define osFchown    ((int(*)(int,uid_t,gid_t))aSyscall[20].pCurrent)

#if defined(HAVE_FCHOWN)
  { "geteuid",      (sqlite3_syscall_ptr)geteuid,         0 },
#else
  { "geteuid",      (sqlite3_syscall_ptr)0,               0 },
#endif
#define osGeteuid   ((uid_t(*)(void))aSyscall[21].pCurrent)

#if !defined(SQLITE_OMIT_WAL) || SQLITE_MAX_MMAP_SIZE>0
  { "mmap",         (sqlite3_syscall_ptr)mmap,            0 },
#else
  { "mmap",         (sqlite3_syscall_ptr)0,               0 },
#endif
#define osMmap ((void*(*)(void*,size_t,int,int,int,off_t))aSyscall[22].pCurrent)

#if !defined(SQLITE_OMIT_WAL) || SQLITE_MAX_MMAP_SIZE>0
  { "munmap",       (sqlite3_syscall_ptr)munmap,          0 },
#else
  { "munmap",       (sqlite3_syscall_ptr)0,               0 },
#endif
#define osMunmap ((int(*)(void*,size_t))aSyscall[23].pCurrent)

#if HAVE_MREMAP && (!defined(SQLITE_OMIT_WAL) || SQLITE_MAX_MMAP_SIZE>0)
  { "mremap",       (sqlite3_syscall_ptr)mremap,          0 },
#else
  { "mremap",       (sqlite3_syscall_ptr)0,               0 },
#endif
#define osMremap ((void*(*)(void*,size_t,size_t,int,...))aSyscall[24].pCurrent)

#if !defined(SQLITE_OMIT_WAL) || SQLITE_MAX_MMAP_SIZE>0
  { "getpagesize",  (sqlite3_syscall_ptr)unixGetpagesize, 0 },
#else
  { "getpagesize",  (sqlite3_syscall_ptr)0,               0 },
#endif
#define osGetpagesize ((int(*)(void))aSyscall[25].pCurrent)

#if defined(HAVE_READLINK)
  { "readlink",     (sqlite3_syscall_ptr)readlink,        0 },
#else
  { "readlink",     (sqlite3_syscall_ptr)0,               0 },
#endif
#define osReadlink ((ssize_t(*)(const char*,char*,size_t))aSyscall[26].pCurrent)

#if defined(HAVE_LSTAT)
  { "lstat",         (sqlite3_syscall_ptr)lstat,          0 },
#else
  { "lstat",         (sqlite3_syscall_ptr)0,              0 },
#endif
#define osLstat      ((int(*)(const char*,struct stat*))aSyscall[27].pCurrent)

#if defined(__linux__) && defined(SQLITE_ENABLE_BATCH_ATOMIC_WRITE)
# ifdef __ANDROID__
  { "ioctl", (sqlite3_syscall_ptr)(int(*)(int, int, ...))ioctl, 0 },
# else
  { "ioctl",         (sqlite3_syscall_ptr)ioctl,          0 },
# endif
#else
  { "ioctl",         (sqlite3_syscall_ptr)0,              0 },
#endif
#define osIoctl ((int(*)(int,int,...))aSyscall[28].pCurrent)

}; /* End of the overrideable system calls */



class StorageServer
{
public:


    /*sqlite3_file server function*/
    static int unixClose(sqlite3_file *id);
    static int unixRead(sqlite3_file *id, void *pBuf, int amt,sqlite3_int64 offset);
    static int unixTruncate(sqlite3_file *id, i64 nByte);
    static int unixSync(sqlite3_file *id, int flags);
    static int unixFileSize(sqlite3_file *id, i64 *pSize);
    static int unixLock(sqlite3_file *id, int eFileLock);
    static int unixUnlock(sqlite3_file *id, int eFileLock);
    static int unixCheckReservedLock(sqlite3_file *id, int *pResOut);
    static int unixDeviceCharacteristics(sqlite3_file *id);
    static int unixShmMap(sqlite3_file *fd,int iRegion,int szRegion,int bExtend,void volatile **pp);
    static int unixShmLock(sqlite3_file *fd,int ofst,int n,int flags);
    static void unixShmBarrier(sqlite3_file *fd);
    static int unixFetch(sqlite3_file *fd, i64 iOff, int nAmt, void **pp);
    static int unixUnfetch(sqlite3_file *fd, i64 iOff, void *p);

    /*sqlite3 vfs server function*/
    
private:

    StorageServer();
    StorageServer(StorageServer &&) = default;
    StorageServer(const StorageServer &) = default;
    StorageServer &operator=(StorageServer &&) = default;
    StorageServer &operator=(const StorageServer &) = default;
    ~StorageServer();

};

/*
** The remoteFile structure is subclass of sqlite3_file specific to the remote
** VFS implementations.
*/
typedef struct RemoteFile RemoteFile;
struct RemoteFile
{
  sqlite3_io_methods const *  sqlite3_io_methods const *pMethod;  /* Always the first entry */
  sqlite3_vfs *pVfs;                  /* The VFS that created this remote file */
  
  unixInodeInfo *pInode;              /* Info about locks on this inode */
  int h;                              /* The file descriptor */
  unsigned char eFileLock;            /* The type of lock held on this fd */
  unsigned short int ctrlFlags;       /* Behavioral bits.  UNIXFILE_* flags */
  int lastErrno;                      /* The unix errno from last I/O error */
  void *lockingContext;               /* Locking style specific state */
  UnixUnusedFd *pPreallocatedUnused;  /* Pre-allocated UnixUnusedFd */
  const char *zPath;                  /* Name of the file */
  unixShm *pShm;                      /* Shared memory segment information */
  int szChunk;                        /* Configured by FCNTL_CHUNK_SIZE */
#if SQLITE_MAX_MMAP_SIZE>0
  int nFetchOut;                      /* Number of outstanding xFetch refs */
  sqlite3_int64 mmapSize;             /* Usable size of mapping at pMapRegion */
  sqlite3_int64 mmapSizeActual;       /* Actual size of mapping at pMapRegion */
  sqlite3_int64 mmapSizeMax;          /* Configured FCNTL_MMAP_SIZE value */
  void *pMapRegion;                   /* Memory mapped region */
#endif
  int sectorSize;                     /* Device sector size */
  int deviceCharacteristics;          /* Precomputed device characteristics */
};


StorageServer::StorageServer()
{
}

StorageServer::~StorageServer()
{
}

#endif // !1