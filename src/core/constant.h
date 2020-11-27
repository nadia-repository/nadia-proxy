#define IS_MOCK 1

#define DEBUG_INFO 0

#ifdef __APPLE__
    #define FILE_FLAG O_CREAT|O_TRUNC|O_WRONLY
#elif __linux__
    #define FILE_FLAG O_CREAT|O_TRUNC|O_WRONLY|F_SHLCK
#endif