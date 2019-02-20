#include "types.h"
#include "stat.h"
#include "user.h"
#define size 16

short arr[size];
int parent_pid;
int num;

void signal_handler(int mean){
	printf(1, "Mean : %d, %d\n", mean, arr[num*size/8]);
	int varsum = 0;
	for(int i = num*size/8; i < (num+1)*size/8; i++){
		varsum += (mean - arr[i])*(mean - arr[i]);
	}
	char* msg = (char *)malloc(8);
	msg = "0000000\0";
	msg[6] = varsum%10 + '0';
	msg[5] = (varsum/10)%10 + '0';
	msg[4] = (varsum/100)%10 + '0';
	msg[3] = (varsum/100)%10 + '0';
	send(getpid(), parent_pid, msg);
	exit();
}	

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

	char c = (char)'0';
	int fd = open(filename, 0);
	printf(1, "filename %s", filename);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]= c - '0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);

	for(int i = 0; i < size; i++){
		printf(1,"%d\n", arr[i]);
	}

	int mean;
  
  	//----FILL THE CODE HERE for unicast sum and multicast variance

	int child_pids[8];

	parent_pid = getpid();
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
		sigset((sig_handler)&signal_handler);
		printf(1, "Child proc %d\n", getpid());
		char* msg = (char *)malloc(8);
		msg = "0000000\0";
		int partial_sum = 0;
		while(recv(msg) == -1){};
		num = atoi(msg);
		for(int i = num*size/8; i < (num+1)*size/8; i++){
			partial_sum += arr[i];
		}
		msg = "0000000\0";
		msg[6] = partial_sum%10 + '0';
		msg[5] = (partial_sum/10)%10 + '0';
		msg[4] = (partial_sum/100)%10 + '0';
		msg[3] = (partial_sum/100)%10 + '0';
		printf(1, "Partial sum from proc %d is %s from %d\n", getpid(), msg, partial_sum);
		send(getpid(), parent_pid, msg); 
		sleep(10);
		// exit();
	}
	else{
		printf(1, "Parent proc %d", getpid());
		int to;
		char* data = (char *)malloc(8);
		data = "0000000\0";
		// Coordinator process
		for(int i = 0; i < 8; i++){
			// Select ith child process
			to = child_pids[i];
			printf(1, "Sending to proc %d\n", i);
			// Send number of process
			data[6] = i + '0';
			send(getpid(), to, data);
		}

		int total = 0;
		int temp = 0;
		// Get results
		for(int i = 0; i < 8; i++){
			recv(data);
			temp = atoi(data);
			printf(1, "Got results %d from %d so total = %d\n", temp, getpid(), total);
			total += temp;
		}
		mean = total / size;
		printf(1, "Mean = %d\n", mean);
		tot_sum = total;

		data = "0000000\0";
		data[6] = mean%10 + '0';
		data[5] = (mean/10)%10 + '0';
		data[4] = (mean/100)%10 + '0';
		data[3] = (mean/100)%10 + '0';
		send_multi(getpid(), child_pids, data);

		total = 0;
		temp = 0;
		// Get results
		for(int i = 0; i < 8; i++){
			recv(data);
			temp = atoi(data);
			printf(1, "Got results %d from %d so total = %d\n", temp, i, total);
			total += temp;
		}

		variance = (float) total / size;

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
