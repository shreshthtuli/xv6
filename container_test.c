#include "types.h"
#include "stat.h"
#include "user.h"

#define procs 3
#define num_containers 2

int parent_pid;
int num;

int main(int argc, char *argv[])
{
	int child_pids[procs];
    
	int num;
	parent_pid = getpid();
	int child_flag = 1;
    int containers[num_containers];

    // Create containers
    for(int i = 0; i < num_containers; i++)
        containers[i] = create_container();

	int pid = -2;
	for(int i = 0; i < procs; i++){
		num = i+1;
		pid = fork();
		if (pid != 0)
			child_pids[i] = pid;
		else
			goto child;
		printf(1, "Init %d\n", pid);
	}

	child_flag = 0;
    num = 0;
	dps();

	child:
    if(child_flag == 1){
		// Child process
		printf(1, "Child proc %d, num %d\n", getpid(), num);
        if(num == 1 || num == num == 2)
            join_container(containers[0]); // First two child procs join container 1
        else if(num == 3)
            join_container(containers[1]); // Third child proc joins container 2

        printf(1, "Child proc %d, num %d\n", getpid(), num);
        proc_stat_container();

		printf(1, "Exiting child!\n");
		exit();
	}
    else{
        // Parent process
        printf(1, "Parent proc %d, num %d\n", getpid(), num);
    }
}
