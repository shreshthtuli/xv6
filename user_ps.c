// MOD-1 : User program to add wo numbers
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int
main ( int argc , char * argv [])
{
    // If you follow the naming convention , the system call
    // name will be sys_ps and you
    // call it by calling the function ps ();
    ps();
    exit () ;
}
