#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(print_count)
SYSCALL(toggle)
SYSCALL(add)
SYSCALL(ps)
SYSCALL(dps)
SYSCALL(send)
SYSCALL(recv)
SYSCALL(send_multi)
SYSCALL(sigset)
SYSCALL(sigret)
SYSCALL(shutdown)
SYSCALL(start_timer)
SYSCALL(end_timer)
SYSCALL(barrier_init)
SYSCALL(barrier)
SYSCALL(create_container)
SYSCALL(destroy_container)
SYSCALL(join_container)
SYSCALL(leave_container)
SYSCALL(proc_stat_container)