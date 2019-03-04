// MOD-1 : User program to shutdown xv6
#include "types.h"
#include "stat.h"
#include "user.h"

int
main ( int argc , char * argv [])
{
    shutdown();
    exit () ;
}
