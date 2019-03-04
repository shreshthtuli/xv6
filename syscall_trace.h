// MOD-1 : Added sys call trace
#include "types.h"
#include "stat.h"

enum trace_state {TRACE_OFF, TRACE_ON} trace;

extern char* syscallnames[];

extern int num_sys_calls;

extern int syscallcounts[];