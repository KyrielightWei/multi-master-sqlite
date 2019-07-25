/**
 * os_remote 初始化编程
 *
 * */
#include "client/os_remote_client.c"

#define OS_REMOTE_DEBUG 0

#define OS_REMOTE_NO_IMPL 1

#if  OS_REMOTE_DEBUG

#include "common/common_sqlite3.c"

#endif


#if OS_REMOTE_NO_IMPL

static int remoteShmMap(
        sqlite3_file *fd,               /* Handle open on database file */
        int iRegion,                    /* Region to retrieve */
        int szRegion,                   /* Size of regions */
        int bExtend,                    /* True to extend file if necessary */
        void volatile **pp              /* OUT: Mapped memory */
) {
    printf("xShmMap not impl");
    return 0;
}

static int remoteShmLock(
        sqlite3_file *fd,          /* Database file holding the shared memory */
        int ofst,                  /* First lock to acquire or release */
        int n,                     /* Number of locks to acquire or release */
        int flags                  /* What to do with the lock */
) {
    printf("xShmLock not impl");
    return 0;
}

static void remoteShmBarrier(
        sqlite3_file *fd                /* Database file holding the shared memory */
) {
    printf("xShmBarrier not impl");
}

static int remoteShmUnmap(
        sqlite3_file *fd,               /* The underlying database file */
        int deleteFlag                  /* Delete shared-memory if true */
) {
    printf("xShmUnmap not impl");
    return 0;
}

static int remoteSetSystemCall(
        sqlite3_vfs *pNotUsed,        /* The VFS pointer.  Not used */
        const char *zName,            /* Name of system call to override */
        sqlite3_syscall_ptr pNewFunc  /* Pointer to new system call value */
) {
    printf("xSetSystemCall not impl");
    return 0;
}

static sqlite3_syscall_ptr remoteGetSystemCall(
        sqlite3_vfs *pNotUsed,
        const char *zName
) {
    printf("remoteGetSystemCall not impl");
    return NULL;
}

static const char *remoteNextSystemCall(sqlite3_vfs *p, const char *zName) {
    printf("remoteNextSystemCall not impl");
    return NULL;
}

#endif



/*
 *sqlite file 方法定义
 *
 * */
#define REMOTE_IOMTETHODS(FINDER, METHOD, VERSION, CLOSE, LOCK, UNLOCK, CKLOCK, SHMMAP)     \
static const sqlite3_io_methods METHOD = {                                   \
   VERSION,                    /* iVersion */                                \
   CLOSE,                      /* xClose */                                  \
   remoteRead,                   /* xRead */                                   \
   remoteWrite,                  /* xWrite */                                  \
   remoteTruncate,               /* xTruncate */                               \
   remoteSync,                    /* xSync */                                   \
   remoteFileSize,               /* xFileSize */                               \
   LOCK,                       /* xLock */                                   \
   UNLOCK,                     /* xUnlock */                                 \
   CKLOCK,                     /* xCheckReservedLock */                      \
   remoteFileControl,            /* xFileControl */                            \
   remoteSectorSize,             /* xSectorSize */                             \
   remoteDeviceCharacteristics,  /* xDeviceCapabilities */                     \
   SHMMAP,                     /* xShmMap */                                 \
   remoteShmLock,                /* xShmLock */                                \
   remoteShmBarrier,             /* xShmBarrier */                             \
   remoteShmUnmap,               /* xShmUnmap */                               \
   remoteFetch,                  /* xFetch */                                  \
   remoteUnfetch,                /* xUnfetch */                                \
};                                                                           \
static const sqlite3_io_methods *FINDER##Impl(const char *z, unixFile *p){   \
  UNUSED_PARAMETER(z); UNUSED_PARAMETER(p);                                  \
  return &METHOD;                                                            \
}                                                                            \
static const sqlite3_io_methods *(*const FINDER)(const char*,unixFile *p)    \
    = FINDER##Impl;

/*
** Here are all of the sqlite3_io_methods objects for each of the
** locking strategies.  Functions that return pointers to these methods
** are also created.
*/
REMOTE_IOMTETHODS(
        remote_posixIoFinder,            /* Finder function name */
        remote_posixIoMethods,           /* sqlite3_io_methods object name */
        3,                        /* shared memory and mmap are enabled */
        remoteClose,                /* xClose method */
        remoteLock,                 /* xLock method */
        remoteUnlock,               /* xUnlock method */
        remoteCheckReservedLock,    /* xCheckReservedLock method */
        remoteShmMap                /* xShmMap method */
)

/**
 * remoteFile <-> unixFile  #pMethods
 */
void setClientRemotePMethods(sqlite3_file *pf)   //recive on client: ->return
{
    pf->pMethods = &remote_posixIoMethods;
}


SQLITE_API int sqlite3_os_init(void) {
#define UNIXVFS(VFSNAME, FINDER) {                        \
    3,                    /* iVersion */                    \
    sizeof(unixFile),     /* szOsFile */                    \
    MAX_PATHNAME,         /* mxPathname */                  \
    0,                    /* pNext */                       \
    VFSNAME,              /* zName */                       \
    (void*)&FINDER,       /* pAppData */                    \
    remoteOpen,             /* xOpen */                       \
    remoteDelete,           /* xDelete */                     \
    remoteAccess,           /* xAccess */                     \
    remoteFullPathname,     /* xFullPathname */               \
    unixDlOpen,           /* xDlOpen */                     \
    unixDlError,          /* xDlError */                    \
    unixDlSym,            /* xDlSym */                      \
    unixDlClose,          /* xDlClose */                    \
    unixRandomness,       /* xRandomness */                 \
    unixSleep,            /* xSleep */                      \
    unixCurrentTime,      /* xCurrentTime */                \
    remoteGetLastError,     /* xGetLastError */               \
    unixCurrentTimeInt64, /* xCurrentTimeInt64 */           \
    remoteSetSystemCall,    /* xSetSystemCall */              \
    remoteGetSystemCall,    /* xGetSystemCall */              \
    remoteNextSystemCall,   /* xNextSystemCall */             \
  }

    /*
     * posixIoFinder指向unixFile的函数指针，可能需要定义自己的指向remote函数的版本
     *
     * */
    static sqlite3_vfs aVfs[] = {
            UNIXVFS("remote_unix", remote_posixIoFinder),
    };
    unsigned int i;          /* Loop counter */

    /* Double-check that the aSyscall[] array has been constructed
    ** correctly.  See ticket [bb3a86e890c8e96ab] */
    assert(ArraySize(aSyscall) == 29);
    /* Register all VFSes defined in the aVfs[] array */
    for (i = 0; i < (sizeof(aVfs) / sizeof(sqlite3_vfs)); i++) {
        sqlite3_vfs_register(&aVfs[i], i == 0);
    }

    remote_sqlite3_os_init();
    //unixBigLock = sqlite3MutexAlloc(SQLITE_MUTEX_STATIC_VFS1);
    return SQLITE_OK;
}

int sqlite3_os_end(void) {
    //unixBigLock = 0;
    return SQLITE_OK;
}
 






