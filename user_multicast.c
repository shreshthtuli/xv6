#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8

void interruptHandler(char* msg){
	printf(1, "Interrupt msg received : %s\n", msg);
}


int main(void)
{
	printf(1,"%s\n","IPC Test case");

	sigset((sig_handler)&interruptHandler);

	int cid = fork();
	if(cid==0){
		// This is child
		for(int i = 0; i < 10; i++){
			sleep(1);
		}
		exit();
	}else{
		// This is parent
		char *msg_child = (char *)malloc(MSGSIZE);
		msg_child = "hello!!!";
		int arr[1] = { cid };
		send_multi(getpid(), *arr, msg_child);	
		printf(1,"1 PARENT: msg sent is: %s \n", msg_child );
		free(msg_child);
	}
	
	exit();
}

