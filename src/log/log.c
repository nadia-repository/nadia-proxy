#include "log.h"


LOG_INSTANCE *init_log_file(char *dir){
    LOG_INSTANCE *log_instance = malloc(sizeof(LOG_INSTANCE));
    log_instance->log_file_map = INIT_HASHMAP;

    LOG_FILE *nadia_log_file = malloc(sizeof(LOG_FILE));
    nadia_log_file->name = "nadia_log";
    nadia_log_file->dir = dir;
    nadia_log_file->log_type = NADIA_LOG;
    nadia_log_file->fp = NULL;
    PUT_HASHMAP(log_instance->log_file_map,nadia_log_file->log_type,nadia_log_file);

    LOG_FILE *error_log_file = malloc(sizeof(LOG_FILE));
    error_log_file->name = "error_log";
    error_log_file->dir = dir;
    error_log_file->log_type = ERROR_LOG;
    error_log_file->fp = NULL;
    PUT_HASHMAP(log_instance->log_file_map,error_log_file->log_type,error_log_file);

    LOG_FILE *access_log_file = malloc(sizeof(LOG_FILE));
    access_log_file->name = "access_log";
    access_log_file->dir = dir;
    access_log_file->log_type = ACCESS_LOG;
    access_log_file->fp = NULL;
    PUT_HASHMAP(log_instance->log_file_map,access_log_file->log_type,access_log_file);

    return log_instance;
}

void write_log_file(LOG_TYPE type, LOG_INSTANCE *log_instance){
    LOG_FILE *log_file = (LOG_FILE *)GET_HASHMAP(log_instance->log_file_map,type);

    if(log_file->fp == NULL){

    }

}




 
#define FILE_MAX_SIZE (1024*1024)
 
/*
获得当前时间字符串
@param buffer [out]: 时间字符串
@return 空
*/
void get_local_time(char* buffer){
	time_t rawtime; 
	struct tm* timeinfo;
 
	time(&rawtime); 
	timeinfo = localtime(&rawtime); 
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
		(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}
 
/*
获得文件大小
@param filename [in]: 文件名
@return 文件大小
*/
long get_file_size(char* filename){
	long length = 0;
	FILE *fp = NULL;
 
	fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
	}
 
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
 
	return length;
}
 
/*
写入日志文件
@param filename [in]: 日志文件名
@param max_size [in]: 日志文件大小限制
@param buffer [in]: 日志内容
@param buf_size [in]: 日志内容大小
@return 空
*/
void write_log_file(char* filename, long max_size, char* buffer, unsigned buf_size)
{
	if (filename != NULL && buffer != NULL)
	{
		// 文件超过最大限制, 删除
		long length = get_file_size(filename);
 
		if (length > max_size)
		{
			unlink(filename); // 删除文件
		}
 
		// 写日志
		{
			FILE *fp;
			fp = fopen(filename, "at+");
			if (fp != NULL)
			{
				char now[32];
				memset(now, 0, sizeof(now));
				get_local_time(now);
				fwrite(now, strlen(now)+1, 1, fp);
				fwrite(buffer, buf_size, 1, fp);
 
				fclose(fp);
				fp = NULL;
			}
		}
	}
}

void create_log_file(){

}