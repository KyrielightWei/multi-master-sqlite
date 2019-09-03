#ifndef LOG_INCLUDE
#define LOG_INCLUDE


#ifdef __cplusplus
int log_init(const char * path);
void log_append(int log_num,const char * val);
#endif


#ifdef __cplusplus
extern "C"{
#endif
int log_init_C_API(const char * path);
void log_append_C_API(int log_num,const char * val);
#ifdef __cplusplus
}
#endif

#endif // !LOG_INCLUDE