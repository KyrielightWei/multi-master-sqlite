#ifndef STORAGE_SERVER

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


StorageServer::StorageServer()
{
}

StorageServer::~StorageServer()
{
}

#endif // !1