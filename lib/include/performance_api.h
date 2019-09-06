
#ifndef PERFORMANCE_INCLUDE
#define PERFORMANCE_INCLUDE
#include <stdint.h>

/**
 * Type of performance indicatior value 
*/
#define VALUE_TYPE int64_t

//performance record type 
#define PERFORMANCE_TIME 1

//============================================================
/***
 *  Performance API
 **/

#ifdef __cplusplus

void createIndicatior(const char * dirPath,const char * name);
long long beginIndicatiorTimeRecord(const char * name);
long long endIndicatiorTimeRecord(const char * name);
void addPerformanceRecord(const char * name,VALUE_TYPE val);
void flushNow(const char * name);
void finishRecord(const char * name);

#endif


#ifdef __cplusplus
extern "C"{
#endif
void createIndicatior_C_API(const char * dirPath,const char * name);
long long beginIndicatiorTimeRecord_C_API(const char * name);
long long endIndicatiorTimeRecord_C_API(const char * name);
void addPerformanceRecord_C_API(const char * name,VALUE_TYPE val);
void flushNow_C_API(const char * name);
void finishRecord_C_API(const char * name);
#ifdef __cplusplus
}
#endif

//============================================================

#endif // PERFORMANCE_INCLUDE
