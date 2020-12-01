#include "proxy.h"
#include "hashmap.h"
#include "thread.h"
#ifdef __linux__
    #include <sys/prctl.h>
#endif

void do_work();