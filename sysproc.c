#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall_trace.h"
#include "spinlock.h"

#define num_message_buffers 20
#define message_size 8

typedef void (* sig_handler) (char* msg);

char buffers[num_message_buffers][8] = { "        " };
int from_pids[20] = { -1 };
int to_pids[20] = { -1 };


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// MOD-1 : Added syscall printing with counts
int
sys_print_count(void)
{
  for(uint i = 0; i < num_sys_calls; i++){
    cprintf("%s %d\n", syscallnames[i], syscallcounts[i]);
  }
  return 0;
}

// MOD-1 : Toggle the tracer on or off
int
sys_toggle(void)
{
  trace = 1 - trace;
  // MOD-1 : Reset all counts
  if(trace == 1){
    for(uint i = 0; i < num_sys_calls; i++){
      syscallcounts[i] = 0;
    }
  }
  return 0;
}

// MOD-1 : Syscall to add two numbera
int
sys_add(int a, int b)
{
  argint(0, &a);
  argint(1, &b);
  return a+b;
}

// MOD-1 : System call to show running processes
void process_status(void);
int
sys_ps(void)
{
  process_status();
  return 0;
}

// MOD-1 : System call to show running processes (detailed) Extra
void process_status_detailed(void);
int
sys_dps(void)
{
  process_status_detailed();
  return 0;
}

// MOD-1 : System call to send message of 8 bytes
struct spinlock lock;
int
sys_send(int sender_pid, int rec_pid, void *msg)
{
  argint(0, &sender_pid);
  argint(1, &rec_pid);
  argptr(2, (char**)&msg, message_size);
  acquire(&lock);
  for(int i = 0; i < num_message_buffers; i++){
    if(to_pids[i] == -1){
      cprintf("Enter send\n");
      memmove(buffers[i], msg, message_size);
      from_pids[i] = sender_pid;
      to_pids[i] = rec_pid;
      release(&lock);
      cprintf("Exit send : %s\n", buffers[i]);
      return 0;
    }    
  }
  release(&lock);
  return -1;
}

// MOD-1 : System call to receive message
int
sys_recv(void *msg)
{
  argptr(0, (char**)&msg, message_size);
  int me = myproc()->pid;
  acquire(&lock);
  for(int i = 0; i < num_message_buffers; i++){
    if(to_pids[i] == me){
      memmove(msg, buffers[i], message_size);
      to_pids[i] = -1;
      release(&lock);
      return 0;
    }    
  }
  release(&lock);
  return -1;
}


// MOD-1 : System call to send message in multi-cast
int sigsend(int dest_pid, char* msg);
int
sys_send_multi(int sender_pid, int rec_pids[], void *msg)
{
  argint(0, &sender_pid);
  argptr(1, (char**)rec_pids, sizeof(*rec_pids)/sizeof(rec_pids[0]));
  argptr(2, (char**)&msg, message_size);
  acquire(&lock);
  int result = 0;
  for(int t = 0; t < sizeof(*rec_pids)/sizeof(rec_pids[0]); t++){
    int to = rec_pids[t];
    result = sigsend(to, msg);
    if(result < 0){
      release(&lock);
      return -1;
    }
  }
  release(&lock);
  return 0;
}

// MOD-1 : System call to set interrupt handler
int sigset(sig_handler new_sighandler);
int
sys_sigset(sig_handler func)
{
  int sighandler;
  argint(0, &sighandler);
  sigset((sig_handler) sighandler);
  return 1;
}

// MOD-1 : Syscall for returning from trap
int sigret(void);
int
sys_sigret(void)
{
  sigret();
  return 0;
}
