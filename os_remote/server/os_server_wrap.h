//
// Created by rrzhang on 19-7-8.
//

#ifndef OS_REMOTE_OS_SERVER_WRAP_H
#define OS_REMOTE_OS_SERVER_WRAP_H

void WrapInit(const char *argIn, char *argOut);
void WrapOpen(const char *argIn, char *argOut);
void WrapDelete(const char *argIn, char *argOut);
void WrapAccess(const char *argIn, char *argOut);
void WrapFullPathname(const char *argIn, char *argOut);
void WrapRandomness(const char *argIn, char *argOut);
void WrapSleep(const char *argIn, char *argOut);
void WrapCurrentTime(const char *argIn, char *argOut);
void WrapGetLastError(const char *argIn, char *argOut);
void WrapCurrentTimeInt64(const char *argIn, char *argOut);
void WrapWrite(const char *argIn, char *argOut);
void WrapRead(const char *argIn, char *argOut);
void WrapTruncate(const char *argIn, char *argOut);
void WrapSync(const char *argIn, char *argOut);
void WrapFileSize(const char *argIn, char *argOut);
void WrapFileControl(const char *argIn, char *argOut);
void WrapSectorSize(const char *argIn, char *argOut);
void WrapDeviceCharacteristics(const char *argIn, char *argOut);
void WrapClose(const char *argIn, char *argOut);
void WrapLock(const char *argIn, char *argOut);
void WrapUnlock(const char *argIn, char *argOut);
void WrapCheckReservedLock(const char *argIn, char *argOut);



#endif //OS_REMOTE_OS_SERVER_WRAP_H
