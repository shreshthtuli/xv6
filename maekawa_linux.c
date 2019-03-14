#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <math.h>
#include <stdlib.h>
#include <time.h> 

#define max_queue_elements 100

#define P 4
#define P1 0
#define P2 2
#define P3 2

int parent_pid;
int msgids[5][5] = {0};
int pbar, pbar2;
int i,j;
int size = 0;
int dat;

typedef struct{
  int queue[max_queue_elements];
  int head;
  int tail;
} simple_queue;

simple_queue q = {
  .head = 0,
  .tail = 0,
  .queue = { 0 },
};

void enque(int val)
{
  q.queue[q.tail] = val;
  q.tail = (q.tail + 1) % max_queue_elements;
}

int deque()
{
  if(q.head == q.tail)
    return -1; // Empty queue
  int result = q.queue[q.head];
  q.head = (q.head + 1) % max_queue_elements;
  return result;
}

// structure for message queue 
struct mesg_buffer { 
    long mesg_type; 
    int mesg_int; 
} message; 

void send(int sender_pid, int receiver_i, int receiver_j, int value)
{
    message.mesg_type = 1; 
    // printf("Sending %f to %d from %d\n", *value, receiver_index, sender_pid);
    message.mesg_int = value;
    int msgid = msgids[receiver_i][receiver_j];
    msgsnd(msgid, &message, sizeof(message), 0);
}

int recv()
{
    int msgid = msgids[i][j];
    msgrcv(msgid, &message, sizeof(message), 1, 0); //printf(message.mesg_text);
    // printf("Got value %d by %d,%d\n", atoi(message.mesg_text), i,j);
    return message.mesg_int;
}

void barrier(){
    if(i == 0 && j == 0){
        printf("first barrier %d %d\n", i, j);
        for(int i = 1; i < P; i++){
            msgrcv(pbar, &message, sizeof(message), 100, 0);
            printf("Got %d\n", message.mesg_type);
        }
        for(int i = 1; i < P; i++){
            message.mesg_type = 200; printf("Resume %d\n", message.mesg_type);
            msgsnd(pbar2, &message, sizeof(message), 0);
        }
    }
    else{
        message.mesg_type = 100;
        printf("bar enter %d,%d\n", i, j);
        msgsnd(pbar, &message, sizeof(message), 0);
        msgrcv(pbar2, &message, sizeof(message), 200, 0); 
        printf("bar exit %d,%d\n", i, j);
    }
}

int main(int argc, char *argv[])
{
    int proc_pids[5][5];
    int pid;
    parent_pid = getpid();
    int wait_time = 0;
    int num_procs_done = 0;
    // int set[9];
    char fname[8] = "./qs/p00";
    // printf("Start\n");
    pbar =  msgget(ftok("./qs/pbar", 65), 0666 | IPC_CREAT);
    pbar2 =  msgget(ftok("./qs/pbar2", 65), 0666 | IPC_CREAT);
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            fname[6] = i + '0'; fname[7] = j = '0';
            msgids[i][j] = msgget(ftok(fname, 65), 0666 | IPC_CREAT); // Creates msg queue and returns identifier for each proc
        }
    }

    printf("Start\n");

    switch (P){
        case 4: size = 2; break;
        case 9: size = 3; break;
        case 16: size = 4; break;
        case 25: size = 5;
    }

	// Initialise procs
    pid = getpid();
	for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            proc_pids[i][j] = pid;
            pid++;
        }
    }
    
    for(i = 1; i < P; i++){
		pid = fork();
		if (pid == 0)
			goto child;
	}

	// dps();

	child:

    proc_pids[0][0] = parent_pid;

    // Find my i,j
	for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            if(proc_pids[i][j] == getpid())
                goto next;
    
    next:
    printf("Found (%d, %d) as my pid = %d\n", i, j, getpid());

    // Set my wait_time
    if(size*i + j < P1 + P2)
        wait_time = 2;

    // Send request to quorum - msg = my pid
    for(int p = 0; p < size; p++){
        for(int q = 0; q < size; q++){
            if(p == i && q == j){
                for(int x = 0; x < size; x++){
                    for(int y = 0; y < size; y++){
                        if(x == i || y == j){
                            int dat = size*i + j;
                            send(getpid(), x, y, dat);
                            printf("Request(%d,%d)-%d to (%d,%d)\n", i, j, dat, x, y);
                        }
                    }
                }
            }
            barrier();

        }
    }
    // printf("second barrier start %d,%d\n", i, j);

    barrier();

    // printf("second barrier end %d,%d\n", i, j);

    // Check all requests
    int temp;
    temp = recv();
    send(getpid(), temp/size, temp%size, temp);
    // printf(1, "Reply to(%d) by %d\n", temp, getpid());
    for(int p = 0; p < 2*size - 2; p++){
        temp = recv();
        if(temp == -1)
            break;
        enque(temp);
        printf("Enq temp = %d,  %d,%d by %d,%d\n", temp, temp/size, temp%size, i,j);
    }

    // printf("third barrier start %d,%d\n", i, j);

    barrier();

    // printf("third barrier end %d,%d\n", i, j);

    // Wait for replies from all procs in quorum - msg = my pid
    int num = 0;
    while(1){
        temp = recv();
        if(temp == -1)
            continue;
        else if(temp/5 == i && temp%5 == j)
            num++;
        else if(temp == 100){
            temp = deque();
            send(getpid(), temp/size, temp%size, temp);
            num_procs_done++;
            // printf(1, "Reply(%d) by %d\n", temp, getpid());
        }
        else{
            enque(temp);
        }

        // printf(1, "Pid %d num = %d\n", getpid(), num);
        
        if(num == (2*size - 1))
            break;
    }

    if(size*i + j < P1)
        goto nosend;

    // Execute critical section
    printf("%d acquired the lock at time %d\n", getpid(), clock());
    sleep(wait_time);
    printf("%d released the lock at time %d\n", getpid(), clock());

    nosend:

    temp = 100; // 100 signifies release
    proc_pids[0][0] = parent_pid;

    // Send release to quorum
    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            if(x == i || y == j){
                send(getpid(), x, y, temp);
                // printf(1, "Release to %d\n", proc_pids[x][y]);
            }
        }
    }

    // Send replies to others
    while(1){
        temp = recv();
        if(temp == -1)
            continue;
        else if(temp == 100){
            temp = deque();
            send(getpid(), temp/5, temp%5, temp);
            num_procs_done++;
        }
        // printf(1, "Numprocs done at %d is %d\n", proc_pids[i][j], num_procs_done);

        if(num_procs_done >= 2*size - 1)
            break;
    }


    if(getpid() == parent_pid){
        for(int i = 0; i < P-1; i++)
            wait(); // wait for all child procs to exit
    }
	
	exit(0);
}
