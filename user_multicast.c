#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8

void interruptHandler(int msg){
	printf(1, "Interrupt msg received : %d\n", msg);
	printf(1, "Done!");
}


int main(void)
{
	printf(1,"%s\n","IPC Test case");

	sigset((sig_handler)&interruptHandler);

	printf(1, "Signal handler set\n");

	int cid = fork();
	if(cid == 0)
		goto child;
	int cid2 = fork();
	if(cid2 == 0)
		goto child;
	int cid3 = fork();
	child:
	if(cid==0 || cid2==0){
		// This is child
		sigset((sig_handler)&interruptHandler);
		for(int i = 0; i < 1; i++){
			sleep(3);
		}
		printf(1, "Exiting child!\n");
		exit();
	}else{
		// This is parent
		dps();
		char *msg_child = (char *)malloc(MSGSIZE);
		msg_child = "0000008\0";
		int arr[] = { cid , cid2 , cid3 };
		// printf(1, "Sending msg\n");
		send_multi(getpid(), arr, msg_child);	
		printf(1,"1 PARENT: msg sent is: %s \n", msg_child );
		free(msg_child);
		wait(); wait(); wait();
	}
	
	exit();
}

