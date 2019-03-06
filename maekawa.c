#include "types.h"
#include "stat.h"
#include "user.h"

#define max_queue_elements 100

#define P 4
#define P1 1
#define P2 2
#define P3 1

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

int main(int argc, char *argv[])
{
	int i,j;
    int proc_pids[5][5];
    int size = 0;
    int pid;
    int parent_pid = getpid();
    // int set[9];

    barrier_init(P);

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

	dps();

	child:

    proc_pids[0][0] = parent_pid;

    // Find my i,j
	for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            if(proc_pids[i][j] == getpid())
                goto next;
    
    next:
    printf(1, "Found (%d, %d) as my pid = %d\n", i, j, getpid());

    // Send request to quorum - msg = my pid
    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            if(x == i || y == j){
                send(getpid(), proc_pids[x][y], &proc_pids[i][j]);
                printf(1, "Request(%d) to %d\n", getpid(), proc_pids[x][y]);
            }
        }
    }

    barrier();

    // Check all requests
    int temp;
    recv(&temp);
    send(getpid(), temp, &temp);
    printf(1, "Reply to(%d) by %d\n", temp, getpid());
    for(int p = 0; p < 2*size - 2; p++){
        recv(&temp);
        if(temp == -1)
            break;
        enque(temp);
        printf(1, "Enq %d by %d\n", temp, getpid());
    }

    barrier();

    // Wait for replies from all procs in quorum - msg = my pid
    int num = 0;
    while(1){
        recv(&temp);
        if(temp == -1)
            continue;
        else if(temp == getpid())
            num++;
        else if(temp == 100){
            temp = deque();
            send(getpid(), temp, &temp);
            printf(1, "Reply(%d) by %d\n", temp, getpid());
        }
        else{
            enque(temp);
        }

        printf(1, "Pid %d num = %d\n", getpid(), num);
        
        if(num == (2*size - 1))
            break;
    }

    // Execute critical section
    printf(1, "%d acquired the lock at time %d\n", getpid(), uptime());
    printf(1, "%d released the lock at time %d\n", getpid(), uptime());

    temp = 100; // 100 signifies release
    proc_pids[0][0] = parent_pid;

    // Send release to quorum
    for(int x = 0; x < size; x++){
        for(int y = 0; y < size; y++){
            if(x == i || y == j){
                send(getpid(), proc_pids[x][y], &temp);
                printf(1, "Release to %d\n", proc_pids[x][y]);
            }
        }
    }

    // Send replies to others
    while(1){
        recv(&temp);
        if(temp == -1)
            continue;
        else if(temp == 100){
            temp = deque();
            send(getpid(), temp, &temp);
        }

    }
	
	exit();
}
