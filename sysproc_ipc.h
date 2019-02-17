// MOD-1 : Added IPC communication
#include "types.h"

#define num_message_buffers 20
#define message_size 8

char buffers[num_message_buffers][8] = { "        " };
int from_pids[20] = { -1 };
int to_pids[20] = { -1 };
