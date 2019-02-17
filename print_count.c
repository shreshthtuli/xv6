// MOD-1 : User program to call print syscall trace
# include "types.h"
# include "user.h"
# include "date.h"

int
main ( int argc , char * argv [])
{
    // If you follow the naming convention , the system call
    // name will be sys_toggle and you
    // call it by calling the function toggle ();
    print_count () ;
    exit () ;
}
