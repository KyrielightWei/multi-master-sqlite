#include "remote_tool.h"


using std::string;


void ConvertStringToBytes(const std::string & mess,char * bytes,u32 bytes_len)
{
    mess.copy(bytes,bytes_len);
    return;
}

void ConvertBytesToString(const char * bytes,std::string & mess,u32 bytes_len)
{
    string temp(bytes,bytes_len);
    mess.swap(temp);
    return;
}

