#include "container.c"

#define procs 5
#define num_containers 3

int parent_pid;
int num;

int main(int argc, char *argv[])
{
	int child_pids[procs];
    
	barrier_init(procs + 1);
	int num;
	parent_pid = getpid();
	int child_flag = 1;
    int containers[num_containers];

    /* Creating the containers . */
    for(int i = 0; i < num_containers; i++){
        containers[i] = create_container();
	}

	int pid = -2;
	/* Multiple process creation to test the scheduler */
	for(int i = 0; i < procs; i++){
		num = i+1;
		pid = fork();
		if (pid != 0)
			child_pids[i] = pid;
		else
			goto child;
		printf(1, "Init %d\n", child_pids[i]);
	}

	child_flag = 0;
    num = 0;
	dps();

	child:
    if(child_flag == 1){
		// Child process
		printf(1, "Child proc %d, num %d\n", getpid(), num);
        if(num == 1 || num == 2 || num == 3)
            join_container(containers[0]); // First three child procs join container 1
        else if(num == 4)
            join_container(containers[1]); // Forth child proc joins container 2
		else if(num == 5)
			join_container(containers[2]); // Fifth proc joins container 3

		barrier();

		/* ---------- PROCESS ISOLATION ---------- */
		// called by atmost one process in every container .
		if(num >= 3)
			proc_stat_container(); // Each container one proc does ps call

		barrier();
		/* ---------- SCHEDULER TEST ---------- */
		scheduler_log_on();
		sleep(10);
		scheduler_log_off();

		barrier();

		/* ---------- MEMORY ISOLATION TEST ---------- */
		printf(1, "Child proc %d, num %d memory log on\n", getpid(), num);
		memory_log_on();
		char* a = (char*)container_malloc(sizeof(char)*10);
		char* b = (char*)container_malloc(sizeof(char)*14);
		char* c = (char*)container_malloc(sizeof(char)*8);
		memory_log_off();

		barrier();

		/* ---------- FILE SYSTEM TEST ---------- */
		if(num >= 3){
			container_ls();
			a = "file_0";
			a[5] = getpid() + '0';
			int fd = container_create(a);
			container_write(fd, "hello_world!\n");
			container_close(fd);
			barrier();
			printf(1, "Child proc %d, num %d creating %s\n", getpid(), num, a);
			container_ls();
			c = "my_file";
			fd = container_create(c);
			b = "Modified by: ";
			b[12] = getpid() + '0'; b[13] = '\n';
			container_write(fd, b);
			container_close(fd);
			container_cat(c); 
			printf(1, "Child proc %d, num %d creating my_file\n", getpid(), num);
			container_ls();
		}
		else{
			a = "file_";
			a[5] = getpid() + '0'; a[6] = '\0';
			container_create(a); c = "my_file\0";
			barrier();
		}

		printf(1, "Exiting child %d, num %d\n", getpid(), num);
		leave_container();
		barrier();
		exit();
	}
    else{
        // Parent process
        printf(1, "Parent proc %d, num %d\n", getpid(), num);
		barrier();
		barrier();
		barrier();
		barrier();
		barrier();
		barrier();
		destroy_container(containers[0]);
		destroy_container(containers[1]);
		destroy_container(containers[2]);
		for(int i = 0; i < procs; i++)
			wait();
    }
}
