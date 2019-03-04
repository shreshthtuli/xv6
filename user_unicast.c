#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8

int main(void)
{
	printf(1,"%s\n","IPC Test case");
	
	int cid = fork();
	if(cid==0){
		// This is child
		int msg;
		recv(&msg);
		// int a = (int)*msg;
		printf(1,"2 CHILD: msg recv is: %s : %d \n", msg, (int)msg );
		exit();
	}else{
		// This is parent
		dps();
		int a = 9;
		// sleep(10);
		send(getpid(),cid, &a);	
		printf(1,"1 PARENT: msg sent is: %d \n", (int)a );
		free(&a);
		wait();
	}
	
	exit();
}

