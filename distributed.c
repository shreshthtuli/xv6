#include "types.h"
#include "stat.h"
#include "user.h"
#define size 16

int
main(int argc, char *argv[])
{
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	
	float variance = 0.0;

	short arr[size];
	char c = '0';
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=0; //c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
	for(int i = 0; i < size; i++){
		printf(1,"%d\n", arr[i]);
	}
  
  	//----FILL THE CODE HERE for unicast sum and multicast variance

	int child_pids[8];

	int parent_pid = getpid();
	int child_flag = 1;

	int pid = -2;
	for(int i = 0; i < 8; i++){
		pid = fork();
		if (pid == 0){
			goto child;
		}
		printf(1, "Init %d\n", pid);
		child_pids[i] = pid;
	}

	child_flag = 0;
	dps();

	child:
	if(child_flag == 1){
		// Child process
		printf(1, "Child proc %d\n", getpid());
		char* msg = (char *)malloc(8);
		msg = "0000000\0";
		int val;
		int partial_sum = 0;
		for(int i = 0; i < size/8; i++){
			recv(msg);
			val = msg[7] - '0';
			printf(1, "msg %s, %d, %d\n", msg, getpid(), val);
			partial_sum += val;
		}
		msg = "0000000\0";
		msg[7] = partial_sum%10 + '0';
		msg[6] = (partial_sum/10)%10 + '0';
		msg[5] = (partial_sum/100)%10 + '0';
		msg[4] = (partial_sum/100)%10 + '0';
		printf(1, "Partial sum from proc %d is %s from %d\n", getpid(), msg, partial_sum);
		send(getpid(), parent_pid, msg); 
		exit();
	}
	else{
		printf(1, "Parent proc %d", getpid());
		int to;
		char* data = (char *)malloc(8);
		data = "0000000\0";
		int index = 0;
		int val;
		// Coordinator process
		for(int i = 0; i < 8; i++){
			// Select ith child process
			to = child_pids[i];
			printf(1, "Sending to proc %d\n", i);
			// Send 250 values to this process
			for(int j = 0; j < size/8; j++){
				val = arr[index];
				data[7] = val + '0';
				printf(1, "Data %s\n", data);
				send(getpid(), to, data);
				index++;
			}
		}

		int total = 0;
		int temp = 0;
		// Get results
		for(int i = 0; i < 8; i++){
			recv(data);
			temp = (data[7] - '0') + 10*(data[6] - '0') + 100*(data[5] - '0') + 1000*(data[4] - '0');
			printf(1, "Got results %d from %d so total = %d\n", temp, getpid(), total);
			total += temp;
		}
		int mean = total / size;
		printf(1, "Mean = %d\n", mean);
		tot_sum = total;
		for(int i = 0; i < 8; i++){
			wait();
		}
	}

	
  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	else{ //mulicast variance
		printf(1,"Variance of array for file %s is %d\n", filename,(int)variance);
	}
	exit();
}
