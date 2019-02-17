#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall_trace.h"
#include "sysproc_ipc.h"
#include "spinlock.h"


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
    if(!to_pids[i] || to_pids[i] == -1){
      memmove(buffers[i], msg, 8);
      from_pids[i] = sender_pid;
      to_pids[i] = rec_pid;
      release(&lock);
      return 0;
    }    
  }
  release(&lock);
  return -1;
}

// MOD-1 : System call to receive message
int sys_recv(void *msg)
{
  argptr(0, (char**)&msg, message_size);
  int me = myproc()->pid;
  acquire(&lock);
  for(int i = 0; i < num_message_buffers; i++){
    if(to_pids[i] == me){
      for(int j = 0; j < message_size; j++){
        *((char*)msg+j) = *(buffers[i]+j);
      }
      to_pids[i] = -1;
      release(&lock);
      return 0;
    }    
  }
  release(&lock);
  return -1;
}
