//
// Created by rrzhang on 19-7-12.
//
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>


static char debugStr[500] = {'\0'};
static char localTime[] = {'\0'};

char *GetTime(){


    time_t t;
    struct tm *lt;
    time(&t);//获取Unix时间戳。
    lt = localtime(&t);//转为时间结构。

    sprintf(localTime, "%d/%d/%d %d:%d:%d   ", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
}

int OpenFile() {
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd > 0) {
        return 0;
    } else {
        return -1;
    }
}

int DebugClient(int i, const char *info) {
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        return -1;
    }

    GetTime();
    write(fd,localTime, strlen(localTime));
    write(fd, info, strlen(info));

//    size_t value_len = strlen(info);
//    while (value_len > 0) {
//        ssize_t r = write(fd, info, value_len);
//        if (r < 0) {
//            if (errno == EINTR) {
//                continue;  // Retry
//            }
//            return -1;
//        }
//        info += r;
//        value_len -= r;
//    }
    return 0;
}

int CloseFile() {
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    close(fd);
}

int TEST_DebugClient() {
    DebugClient(sprintf(debugStr, "---start remoteOpen:\n"), debugStr);
    DebugClient(sprintf(debugStr, "number : %d\n", 3), debugStr);
    DebugClient(sprintf(debugStr, "string : %s\n", "adfgshf"), debugStr);

    return 0;
}

