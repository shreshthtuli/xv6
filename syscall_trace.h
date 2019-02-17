// MOD-1 : Added sys call trace
#include "types.h"
#include "stat.h"

extern int trace;

extern char* syscallnames[];

extern int num_sys_calls;

extern int* syscallcounts[];