//
// Created by rrzhang on 19-7-8.
//

#ifndef OS_REMOTE_OS_SERVER_WRAP_H
#define OS_REMOTE_OS_SERVER_WRAP_H

char *WrapInit(void);
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


#endif //OS_REMOTE_OS_SERVER_WRAP_H
