/**
 * File : some tool function for rpc
 */
#ifndef  REMOTE_TOOL

//#include "sqlite3.c"
#include <string>

//void client_test();

typedef unsigned int u32;

void ConvertStringToBytes(const std::string & mess,char * bytes,u32 bytes_len);

void ConvertBytesToString(const char * bytes,std::string & mess,u32 bytes_len);


#endif // ! REMOTE_TOOL