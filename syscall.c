#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "syscall_trace.h"
#include "syscall_container.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
// MOD-1 : Additional syscalls
extern int sys_print_count(void);
extern int sys_toggle(void);
extern int sys_add(void);
extern int sys_ps(void);
extern int sys_dps(void);
extern int sys_send(void);
extern int sys_recv(void);
extern int sys_send_multi(void);
extern int sys_sigset(void);
extern int sys_sigret(void);
extern int sys_shutdown(void);
extern int sys_start_timer(void);
extern int sys_end_timer(void);
// MOD-2 : Barrier syscalls
extern int sys_barrier_init(void);
extern int sys_barrier(void);
// MOD-3 : Container syscalls
extern int sys_create_container(void);
extern int sys_destroy_container(void);
extern int sys_join_container(void);
extern int sys_leave_container(void);
extern int sys_proc_stat_container(void);
extern int sys_scheduler_log_on(void);
extern int sys_scheduler_log_off(void);
extern int sys_cid(void);
extern int sys_memory_log_on(void);
extern int sys_memory_log_off(void);
extern int sys_memory_gva(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
// MOD-1 : Additional syscalls
[SYS_print_count] sys_print_count,
[SYS_toggle]  sys_toggle,
[SYS_add]     sys_add,
[SYS_ps]      sys_ps,
[SYS_dps]     sys_dps,
[SYS_send]    sys_send,
[SYS_recv]    sys_recv,
[SYS_send_multi] sys_send_multi,
[SYS_sigset]  sys_sigset,
[SYS_sigret]  sys_sigret,
[SYS_shutdown] sys_shutdown,
[SYS_start_timer] sys_start_timer,
[SYS_end_timer] sys_end_timer,
// MOD-2 : Barrier syscalls
[SYS_barrier_init] sys_barrier_init,
[SYS_barrier] sys_barrier,
// MOD-3 : Container syscalls
[SYS_create_container] sys_create_container,
[SYS_destroy_container] sys_destroy_container,
[SYS_join_container] sys_join_container,
[SYS_leave_container]  sys_leave_container,
[SYS_proc_stat_container] sys_proc_stat_container,
[SYS_scheduler_log_on] sys_scheduler_log_on,
[SYS_scheduler_log_off] sys_scheduler_log_off,
[SYS_cid]        sys_cid,
[SYS_memory_log_on]     sys_memory_log_on,
[SYS_memory_log_off]    sys_memory_log_off,
[SYS_memory_gva]        sys_memory_gva
};

// MOD-1 : Definitions of external variables here
enum trace_state trace = TRACE_OFF;

char* syscallnames[] = {
    "sys_fork",
    "sys_exit",
    "sys_wait",
    "sys_pipe",
    "sys_read",
    "sys_kill",
    "sys_exec",
    "sys_fstat",
    "sys_chdir",
    "sys_dup",
    "sys_getpid",
    "sys_sbrk",
    "sys_sleep",
    "sys_uptime",
    "sys_open",
    "sys_write",
    "sys_mknod",
    "sys_unlink",
    "sys_link",
    "sys_mkdir",
    "sys_close",
    "sys_print_count",
    "sys_toggle",
    "sys_add",
    "sys_ps",
    "sys_dps",
    "sys_send",
    "sys_recv",
    "sys_send_multi",
    "sys_sigset",
    "sys_sigret",
    "sys_shutdown",
    "sys_start_timer",
    "sys_end_timer",
    "sys_barrier_init",
    "sys_barrier",
    "sys_create_container",
    "sys_destroy_container",
    "sys_join_container",
    "sys_leave_container",
    "sys_proc_stat_container",
    "sys_scheduler_log_on",
    "sys_scheduler_log_off",
    "sys_cid",
    "sys_memory_log_on",
    "sys_memory_log_off",
    "sys_memory_gva"
};

int num_sys_calls = NELEM(syscallnames);

int syscallcounts[NELEM(syscalls)] = { 0 };

void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // MOD-1 : Increment syscall counts if trace is on
    if(trace == TRACE_ON){
      syscallcounts[num-1] = syscallcounts[num-1] + 1;
    }
    // MOD-1 : Print syscall
    // cprintf("DEBUG : %s %d\n", syscallnames[num-1], syscallcounts[num-1]);
    
    // MOD-3 : Syscall check if allowed from this container
    for(int j = 0; j < 100; j++){
      if(container.notAllowed[curproc->containerID][j] == num)
        cprintf("Syscall %d not allowed in container with ID %d\n",
            num, curproc->containerID);; // syscall not allowed from this container
    }
    
    curproc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
