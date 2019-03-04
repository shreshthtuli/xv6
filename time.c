#include "types.h"
#include "stat.h"
#include "user.h"

int main(int arc, char** argv)
{
	printf(1,"Timing command %s\n", argv[1]);
	
	int cid = fork();
	if(cid==0){
		// This is child
        exec(argv[1], &argv[2]);
        sleep(10000);
		printf(1,"2 CHILD: running \n");
		exit();
	}else{
		// This is parent
        start_timer(cid);
		wait();
        printf(1, "Time takes by command %s is %d ms\n", argv[1], end_timer(cid));
	}	
	exit();
}

