#include "os_remote_server.h"

#define RPC_RUN 0
#define LOCAL_TEST 1

#if LOCAL_TEST
#include<iostream>

int main() {
    return 0;
}

#endif

#if  RPC_RUN

int main()
{
    return 0;
}

#endif


/**
 * common functions
 */
void sqlite3_log(int err_code, const char *zFormat, ...){
  va_list valist;

  va_start(valist,zFormat);
  printf(err_code);
  printf(" : ");
  printf(zFormat);
  printf(va_arg(valist,const char*));
  printf("\n");

  return;
}

/*
** Check a unixFile that is a database.  Verify the following:
**
** (1) There is exactly one hard link on the file
** (2) The file is not a symbolic link
** (3) The file has not been renamed or unlinked
**
** Issue sqlite3_log(SQLITE_WARNING,...) messages if anything is not right.
*/
static void verifyDbFile(RemoteFile *pFile){
  struct stat buf;
  int rc;

  // if( pFile->ctrlFlags & UNIXFILE_NOLOCK ) return;

  rc = osFstat(pFile->h, &buf);
  if( rc!=0 ){
    sqlite3_log(SQLITE_WARNING, "cannot fstat db file %s", pFile->zPath);
    return;
  }
  if( buf.st_nlink==0 ){
    sqlite3_log(SQLITE_WARNING, "file unlinked while open: %s", pFile->zPath);
    return;
  }
  if( buf.st_nlink>1 ){
    sqlite3_log(SQLITE_WARNING, "multiple links to file: %s", pFile->zPath);
    return;
  }
  if( fileHasMoved(pFile) ){
    sqlite3_log(SQLITE_WARNING, "file renamed while open: %s", pFile->zPath);
    return;
  }
}

/*
** Close a file descriptor.
**
** We assume that close() almost always works, since it is only in a
** very sick application or on a very sick platform that it might fail.
** If it does fail, simply leak the file descriptor, but do log the
** error.
**
** Note that it is not safe to retry close() after EINTR since the
** file descriptor might have already been reused by another thread.
** So we don't even try to recover from an EINTR.  Just log the error
** and move on.
*/
static void robust_close(unixFile *pFile, int h, int lineno){
  if( osClose(h) ){
    sqlite3_log(SQLITE_IOERR_CLOSE,"can't close file -- ", lineno); 
  }
}

static int closeFile(RemoteFile *pFile)
{
  //ummap
  if(pFile->h >= 0)
  {
    robust_close(pFile,pFile->h,__LINE__);
    pFile->h=-1;
  }
  //track os interface
  // sqlite3_free(pFile->pPreallocatedUnused)
  memset(pFile,0,sizeof(RemoteFile));
  return SQLITE_OK;
}

/*
** Set the pFile->lastErrno.  Do this in a subroutine as that provides
** a convenient place to set a breakpoint.
*/
static void storeLastErrno(RemoteFile *pFile, int error){
  pFile->lastErrno = error;
}

/*
** Seek to the offset passed as the second argument, then read cnt 
** bytes into pBuf. Return the number of bytes actually read.
**
** NB:  If you define USE_PREAD or USE_PREAD64, then it might also
** be necessary to define _XOPEN_SOURCE to be 500.  This varies from
** one system to another.  Since SQLite does not define USE_PREAD
** in any form by default, we will not attempt to define _XOPEN_SOURCE.
** See tickets #2741 and #2681.
**
** To avoid stomping the errno value on a failed read the lastErrno value
** is set before returning.
*/
static int seekAndRead(RemoteFile *id, sqlite3_int64 offset, void *pBuf, int cnt)
{
  int got;
  int prior = 0;
  assert( cnt==(cnt&0x1ffff) );
  assert( id->h>2 );
  do
  {
    if( got==cnt ) break;
    if( got<0 ){
      if( errno==EINTR ){ got = 1; continue; }
      prior = 0;
      storeLastErrno((RemoteFile *)id,  errno);
      break;
    }else if( got>0 ){
      cnt -= got;
      offset += got;
      prior += got;
      pBuf = (void*)(got + (char*)pBuf);
    }
  }while( got>0 );
  return got+prior;
}


/*
**
** This function - unixLogErrorAtLine(), is only ever called via the macro
** unixLogError().
**
** It is invoked after an error occurs in an OS function and errno has been
** set. It logs a message using sqlite3_log() containing the current value of
** errno and, if possible, the human-readable equivalent from strerror() or
** strerror_r().
**
** The first argument passed to the macro should be the error code that
** will be returned to SQLite (e.g. SQLITE_IOERR_DELETE, SQLITE_CANTOPEN). 
** The two subsequent arguments should be the name of the OS function that
** failed (e.g. "unlink", "open") and the associated file-system path,
** if any.
*/
#define unixLogError(a,b,c)     unixLogErrorAtLine(a,b,c,__LINE__)
static int unixLogErrorAtLine(
  int errcode,                    /* SQLite error code */
  const char *zFunc,              /* Name of OS function that failed */
  const char *zPath,              /* File path associated with error */
  int iLine                       /* Source line number where error occurred */
){
  char *zErr;                     /* Message from strerror() or equivalent */
  int iErrno = errno;             /* Saved syscall error number */

  if( zPath==0 ) zPath = "";
  sqlite3_log(errcode,
      "os_unix.c:%d: (%d) %s(%s) - %s",
      iLine, iErrno, zFunc, zPath, zErr
  );

  return errcode;
}


/**
 * end of common functions
 */



/**
 * class StorageServer : functions 
 */
static int StorageServer::unixClose(sqilte3_file * id)
{
    int rc = SQLITE_OK;
    RemoteFile * pFile = (RemoteFile*)id;
    verifyDbFile(id);
    //unlock file
      //enter mutex
    //close file
    rc = closeFile(pFile);
      //leave mutex
    return rc;
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int StorageServer::unixRead(sqlite3_file *id, void *pBuf, int amt,sqlite3_int64 offset)
{
  RemoteFile * pFile = (RemoteFile *)id;
  int got;
  assert( id );
  assert( offset>=0 );
  assert( amt>0 );
  //deal with mmap
  got = seekAndRead(pFile, offset, pBuf, amt);
  if( got==amt ){
    return SQLITE_OK;
  }else if( got<0 ){
    /* lastErrno set by seekAndRead */
    return SQLITE_IOERR_READ;
  }else{
    storeLastErrno(pFile, 0);   /* not a system error */
    /* Unread parts of the buffer must be zero-filled */
    memset(&((char*)pBuf)[got], 0, amt-got);
    return SQLITE_IOERR_SHORT_READ;
  }

}

static int  StorageServer::unixTruncate(sqlite3_file *id, i64 nByte)
{
  RemoteFile * pFile = (RemoteFile *)id;
  int rc;
  assert(pFile);

  if( pFile->szChunk>0 ){
    nByte = ((nByte + pFile->szChunk - 1)/pFile->szChunk) * pFile->szChunk;
  }

  rc = robust_ftruncate(pFile->h, nByte);
  if( rc ){
    storeLastErrno(pFile, errno);
    return unixLogError(SQLITE_IOERR_TRUNCATE, "ftruncate", pFile->zPath);
  }else{
    //mmap
    return SQLITE_OK;
  }
}

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

 /**
  * end of class StorageServer : functions 
 */