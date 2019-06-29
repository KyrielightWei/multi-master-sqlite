
#include "sqliteInt.h"

#define SQLITE_OS_REMOTE 1

#if SQLITE_OS_REMOTE

//======================rpc_test start
#define RPC_TEST 0
#if RPC_TEST

int main() {
    return 0;
}

#endif

//======================rpc_test end


/*
** Maximum supported path-length.
*/
#define MAX_PATHNAME 512

/**
* Rmote sqlitefile function (client) Start
 */

/*
** Close a file.
*/
static int unixClose(sqlite3_file *id){
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int unixRead(
  sqlite3_file *id, 
  void *pBuf, 
  int amt,
  sqlite3_int64 offset
){}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int unixWrite(
  sqlite3_file *id, 
  const void *pBuf, 
  int amt,
  sqlite3_int64 offset 
){}

/*
** Truncate an open file to a specified size
*/
static int unixTruncate(sqlite3_file *id, i64 nByte){}

/*
** Make sure all writes to a particular file are committed to disk.
**
** If dataOnly==0 then both the file itself and its metadata (file
** size, access time, etc) are synced.  If dataOnly!=0 then only the
** file data is synced.
**
** Under Unix, also make sure that the directory entry for the file
** has been created by fsync-ing the directory that contains the file.
** If we do not do this and we encounter a power failure, the directory
** entry for the journal might not exist after we reboot.  The next
** SQLite to access the file will not know that the journal exists (because
** the directory entry for the journal was never created) and the transaction
** will not roll back - possibly leading to database corruption.
*/
static int unixSync(sqlite3_file *id, int flags){}

/*
** Determine the current size of a file in bytes
*/
static int unixFileSize(sqlite3_file *id, i64 *pSize){}

/*
* file lock logic
 */
static int unixLock(sqlite3_file *id, int eFileLock){}
static int unixUnlock(sqlite3_file *id, int eFileLock){}

/*
** This routine checks if there is a RESERVED lock held on the specified
** file by this or any other process. If such a lock is held, set *pResOut
** to a non-zero value otherwise *pResOut is set to zero.  The return value
** is set to SQLITE_OK unless an I/O error occurs during lock checking.
*/
static int unixCheckReservedLock(sqlite3_file *id, int *pResOut){}

/*
** Information and control of an open file handle.
*/
static int unixFileControl(sqlite3_file *id, int op, void *pArg){}

/*
** Return the sector size in bytes of the underlying block device for
** the specified file. This is almost always 512 bytes, but may be
** larger for some devices.
**
** SQLite code assumes this function cannot fail. It also assumes that
** if two files are created in the same file-system directory (i.e.
** a database and its journal file) that the sector size will be the
** same for both.
*/
static int unixSectorSize(sqlite3_file *id){}

/*
** Return the device characteristics for the file.
**
** This VFS is set up to return SQLITE_IOCAP_POWERSAFE_OVERWRITE by default.
** However, that choice is controversial since technically the underlying
** file system does not always provide powersafe overwrites.  (In other
** words, after a power-loss event, parts of the file that were never
** written might end up being altered.)  However, non-PSOW behavior is very,
** very rare.  And asserting PSOW makes a large reduction in the amount
** of required I/O for journaling, since a lot of padding is eliminated.
**  Hence, while POWERSAFE_OVERWRITE is on by default, there is a file-control
** available to turn it off and URI query parameter available to turn it off.
*/
static int unixDeviceCharacteristics(sqlite3_file *id){}

/*
** This function is called to obtain a pointer to region iRegion of the 
** shared-memory associated with the database file fd. Shared-memory regions 
** are numbered starting from zero. Each shared-memory region is szRegion 
** bytes in size.
**
** If an error occurs, an error code is returned and *pp is set to NULL.
**
** Otherwise, if the bExtend parameter is 0 and the requested shared-memory
** region has not been allocated (by any client, including one running in a
** separate process), then *pp is set to NULL and SQLITE_OK returned. If 
** bExtend is non-zero and the requested shared-memory region has not yet 
** been allocated, it is allocated by this function.
**
** If the shared-memory region has already been allocated or is allocated by
** this call as described above, then it is mapped into this processes 
** address space (if it is not already), *pp is set to point to the mapped 
** memory and SQLITE_OK returned.
*/
static int unixShmMap(
  sqlite3_file *fd,               /* Handle open on database file */
  int iRegion,                    /* Region to retrieve */
  int szRegion,                   /* Size of regions */
  int bExtend,                    /* True to extend file if necessary */
  void volatile **pp              /* OUT: Mapped memory */
){}

/*
** Change the lock state for a shared-memory segment.
**
** Note that the relationship between SHAREd and EXCLUSIVE locks is a little
** different here than in posix.  In xShmLock(), one can go from unlocked
** to shared and back or from unlocked to exclusive and back.  But one may
** not go from shared to exclusive or from exclusive to shared.
*/
static int unixShmLock(
  sqlite3_file *fd,          /* Database file holding the shared memory */
  int ofst,                  /* First lock to acquire or release */
  int n,                     /* Number of locks to acquire or release */
  int flags                  /* What to do with the lock */
){}

/*
** Implement a memory barrier or memory fence on shared memory.  
**
** All loads and stores begun before the barrier must complete before
** any load or store begun after the barrier.
*/
static void unixShmBarrier(
  sqlite3_file *fd                /* Database file holding the shared memory */
){}


/*
** If possible, return a pointer to a mapping of file fd starting at offset
** iOff. The mapping must be valid for at least nAmt bytes.
**
** If such a pointer can be obtained, store it in *pp and return SQLITE_OK.
** Or, if one cannot but no error occurs, set *pp to 0 and return SQLITE_OK.
** Finally, if an error does occur, return an SQLite error code. The final
** value of *pp is undefined in this case.
**
** If this function does return a pointer, the caller must eventually 
** release the reference by calling unixUnfetch().
*/
static int unixFetch(sqlite3_file *fd, i64 iOff, int nAmt, void **pp){}

/*
** If the third argument is non-NULL, then this function releases a 
** reference obtained by an earlier call to unixFetch(). The second
** argument passed to this function must be the same as the corresponding
** argument that was passed to the unixFetch() invocation. 
**
** Or, if the third argument is NULL, then this function is being called 
** to inform the VFS layer that, according to POSIX, any existing mapping 
** may now be invalid and should be unmapped.
*/
static int unixUnfetch(sqlite3_file *fd, i64 iOff, void *p){}

 /**
 * End of sqlitefile  function 
  */


/**
* Rmote vfs function (client) 
 */



 /**
 * End of Remote vfs function 
  */



/*
** The remoteFile structure is subclass of sqlite3_file specific to the remote
** VFS implementations.
*/
typedef struct RemoteFile RemoteFile;
struct RemoteFile
{
  sqlite3_io_methods const *  sqlite3_io_methods const *pMethod;  /* Always the first entry */
  sqlite3_vfs *pVfs;                  /* The VFS that created this remote file */
};

static const sqlite3_io_methods remoteIoMethod = {
   3,                    /* iVersion */                                \
   remoteClose,                      /* xClose */                                  \
   remoteRead,                   /* xRead */                                   \
   remoteWrite,                  /* xWrite */                                  \
   remoteTruncate,               /* xTruncate */                               \
   remoteSync,                   /* xSync */                                   \
   remoteFileSize,               /* xFileSize */                               \
   remoteLock,                       /* xLock */                                   \
   remoteUnlock,                     /* xUnlock */                                 \
   remoteReservedLock,         /* xCheckReservedLock */                      \
   remoteFileControl,            /* xFileControl */                            \
   remoteSectorSize,             /* xSectorSize */                             \
   remoteDeviceCharacteristics,  /* xDeviceCapabilities */                     \
   remoteShmMap,                     /* xShmMap */                                 \
   remoteShmLock,                /* xShmLock */                                \
   remoteShmBarrier,             /* xShmBarrier */                             \
   remoteShmUnmap,               /* xShmUnmap */                               \
   remoteFetch,                  /* xFetch */                                  \
   remoteUnfetch,                /* xUnfetch */ 
}



/*
** Initialize the operating system interface.
**
** This routine registers all VFS implementations for unix-like operating
** systems.  This routine, and the sqlite3_os_end() routine that follows,
** should be the only routines in this file that are visible from other
** files.
**
** This routine is called once during SQLite initialization and by a
** single thread.  The memory allocation and mutex subsystems have not
** necessarily been initialized when this routine is called, and so they
** should not be used.
*/
int sqlite3_os_init(void){ 

  #define UNIXVFS(VFSNAME, FINDER) {                        \
    3,                    /* iVersion */                    \
    sizeof(RemoteFile),     /* szOsFile */                    \
    MAX_PATHNAME,         /* mxPathname */                  \
    0,                    /* pNext */                       \
    VFSNAME,              /* zName */                       \
    (void*)&FINDER,       /* pAppData */                    \
    remoteOpen,             /* xOpen */                       \
    remoteDelete,           /* xDelete */                     \
    remoteAccess,           /* xAccess */                     \
    remoteFullPathname,     /* xFullPathname */               \
    remoteDlOpen,           /* xDlOpen */                     \
    remoteDlError,          /* xDlError */                    \
    remoteDlSym,            /* xDlSym */                      \
    remoteDlClose,          /* xDlClose */                    \
    remoteRandomness,       /* xRandomness */                 \
    remoteSleep,            /* xSleep */                      \
    remoteCurrentTime,      /* xCurrentTime */                \
    remoteGetLastError,     /* xGetLastError */               \
    remoteCurrentTimeInt64, /* xCurrentTimeInt64 */           \
    remoteSetSystemCall,    /* xSetSystemCall */              \
    remoteGetSystemCall,    /* xGetSystemCall */              \
    remoteNextSystemCall,   /* xNextSystemCall */             \
  }

  /*
  ** All default VFSes for unix are contained in the following array.
  **
  ** Note that the sqlite3_vfs.pNext field of the VFS object is modified
  ** by the SQLite core when the VFS is registered.  So the following
  ** array cannot be const.
  */
  static sqlite3_vfs aVfs[] = {
    UNIXVFS("remote_linux",          posixIoFinder ),
  };
  unsigned int i;          /* Loop counter */

  /* Double-check that the aSyscall[] array has been constructed
  ** correctly.  See ticket [bb3a86e890c8e96ab] */
  assert( ArraySize(aSyscall)==1 );

  /* Register all VFSes defined in the aVfs[] array */
  for(i=0; i<(sizeof(aVfs)/sizeof(sqlite3_vfs)); i++){
    sqlite3_vfs_register(&aVfs[i], i==0);
  }
 // unixBigLock = sqlite3MutexAlloc(SQLITE_MUTEX_STATIC_VFS1);
  return SQLITE_OK; 
}

/*
** Shutdown the operating system interface.
**
** Some operating systems might need to do some cleanup in this routine,
** to release dynamically allocated objects.  But not on unix.
** This routine is a no-op for unix.
*/
int sqlite3_os_end(void){ 
  return SQLITE_OK; 
}


#endif
