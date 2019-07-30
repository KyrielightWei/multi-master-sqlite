//
// Created by rrzhang on 19-7-12.
//

#ifndef UTIL_C
#define UTIL_C

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>


static char debugStr[500];

static char localTime[100];


void clearStr()
{
    memset(debugStr,0,sizeof(char)*500);
    memset(localTime,0,sizeof(char)*100);
}

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
   // clearStr();
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        return -1;
    }

    GetTime();
    int wr_rc = 0;
    //char rc_str[20];
    //char n = '\n';
    wr_rc = write(fd,localTime, strlen(localTime));
   // write(fd,&n, sizeof(char));
    //sprintf(rc_str,"%d",wr_rc);
   // write(fd,rc_str,sizeof(int));
    wr_rc = write(fd, info, strlen(info));
    //write(fd,&n, sizeof(char));
    //sprintf(rc_str,"%d",wr_rc);
    //write(fd,rc_str,sizeof(int));
    //write(fd,&n, sizeof(char));
    //write(fd,&n, sizeof(char));

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

int DebugChars(const char *info, int size){
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        return -1;
    }

    int wr_rc = 0;

    int count = 0;
    while(count<size){
        char str[5];
        sprintf(str, "%d ", info[count]);
        wr_rc = write(fd, str, strlen(str));
        count++;
    }
    wr_rc = write(fd, "\n", 1);
    return wr_rc;
}

void PrintChars(const char *info, int size){
    int count = 0;
    while(count< size){
        printf("%d ", info[count]);
        count++;
    }
    printf("\n");
}

int CloseFile() {
    int fd = open("LOG", O_APPEND | O_WRONLY | O_CREAT, 0644);
    close(fd);


}

int TEST_DebugClient() {
    int i = sprintf(debugStr, "---start remoteOpen:\n");
    printf("%s",debugStr);
    DebugClient(i, debugStr);
    

    DebugClient(sprintf(debugStr, "number : %d\n", 3), debugStr);
    DebugClient(sprintf(debugStr, "string : %s\n", "adfgshf"), debugStr);

    return 0;
}

// int main() {
//     TEST_DebugClient();
//     return 0;
// }


#endif