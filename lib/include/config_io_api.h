#ifndef FILE_IO_API


#ifdef __cplusplus
void initConfigFile(const char * zpath);
void insertConfigItem(const char * key);
void readConfig(const char * key);
void getConfigVal(const char * key,char * val);
void outputConfig(const char * zpath);
#endif


#ifdef __cplusplus
extern "C" {
#endif
void initConfigFile_C_API(const char * zpath);
void insertConfigItem_C_API(const char * key);
void readConfig_C_API(const char * key);
void getConfigVal_C_API(const char * key,char * val);
void outputConfig_C_API(const char * zpath);
#ifdef __cplusplus
}
#endif


#endif // !FILE_IO_API
