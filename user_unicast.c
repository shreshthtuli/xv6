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
		char *msg = (char *)malloc(MSGSIZE);
		recv(msg);
		int a = (int)*msg;
		printf(1,"2 CHILD: msg recv is: %s : %d \n", msg, a );
		exit();
	}else{
		// This is parent
		char *msg_child = (char *)malloc(MSGSIZE);
		char* a = "aaaaaa\0";
		*a = 9;
		msg_child = a;
		send(getpid(),cid,msg_child);	
		printf(1,"1 PARENT: msg sent is: %s \n", msg_child );
		free(msg_child);
		wait();
	}
	
	exit();
}

