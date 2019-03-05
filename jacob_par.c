#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define fabs(a) (a>=0) ? a : -a

#define N 10
#define EPSILON 0.01
#define procs 2

int parent_pid;
int num;
double diff;
int i,j;
double mean;
double u[N][N];
double w[N][N];

void signal_handler(int mean)
{
    int count = 0; // Count of each
	double diff;
	int w[N][N];
	printf(1, "Child proc %d with num %d got mean %d\n", getpid(), num, mean);
    // Parallelised this - each diff < epsilon should be true
	for(int i = num*(N-2)/procs + 1; i < (num+1)*(N-2)/procs + 1; i++)
		for ( j= 1; j < N-1; j++) u[i][j] = mean;
    for(;;){
        diff = 0.0;
        for(int i = num*(N-2)/procs + 1; i < (num+1)*(N-2)/procs + 1; i++)
            for(j =1 ; j < N-1; j++){
                w[i][j] = ( u[i-1][j] + u[i+1][j]+
                        u[i][j-1] + u[i][j+1])/4.0;
                if( fabs(w[i][j] - u[i][j]) > diff )
                    diff = fabs(w[i][j]- u[i][j]);
            count++;	
            }
        if(diff <= EPSILON) break;
        for(int i = num*(N-2)/procs + 1; i < (num+1)*(N-2)/procs + 1; i++)
            for (j =1; j< N-1; j++) u[i][j] = w[i][j];
    }
	sleep(5*num);

	send(getpid(), parent_pid, &count);
    barrier();
	recv(&count);
	for(int i = num*(N)/procs; i < (num+1)*(N)/procs; i++){
		for(j = 0; j<N; j++){
			printfloat(1,u[i][j]);
			printf(1, ",");
		}
		printf(1,"\n");
	}
	exit();
}

int main(int argc, char *argv[])
{
	// double diff;
	// int i,j;
	// double mean;
	// double u[N][N];
	// double w[N][N];
	int count = 0;
	int child_pids[procs];
	
	mean = 0.0;

	parent_pid = getpid();
	int child_flag = 1;

    barrier_init(procs + 1);

	int pid = -2;
	for(int i = 0; i < procs; i++){
		num = i;
		pid = fork();
		if (pid != 0)
			child_pids[i] = pid;
		else
			goto child;
		printf(1, "Init proc %d with pid %d\n", i, child_pids[i]);
	}

	child_flag = 0;
	dps();

	child:
    if(child_flag == 1){
		// Child process
		sigset((sig_handler)&signal_handler);
		printf(1, "Child proc %d\n", getpid());
		int partial_sum = 0;
		for(int i = 0; i < N; i++){
			u[i][0] = u[i][N-1] = u[0][i] = 100.0;
		    u[N-1][i] = 0.0;
		    partial_sum += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
		}
		printf(1, "Partial sum from proc %d is %d\n", num, partial_sum);
		send(getpid(), parent_pid, &partial_sum); 
		barrier();
        while(1){}; // Wait for mean
		printf(1, "Exiting child!\n");
		exit();
	}
    else{
        int total = 0;
		int temp = 0;
		for(int i = 0; i < N; i++){
			u[i][0] = u[i][N-1] = u[0][i] = 100.0;
		    u[N-1][i] = 0.0;
		}
        // Make sure all procs are done
        barrier();
		// Get results
		for(int i = 0; i < procs; i++){
			recv(&temp);
			total += temp;
			printf(1, "Got results %d from %d so total = %d\n", temp, i, total);
		}
		mean = total / (4 * N * procs);
		printf(1, "Mean : ");
		printfloat(1, mean);
    	
        // Initialise another barrier for next computation
        barrier_init(procs + 1);

		printf(1, "master pid = %d, work sent to pids : %d, %d\n", getpid(),child_pids[0], child_pids[1]);
		// child_pids[0] = 4;

		int mean_send = (int)mean;
        send_multi(parent_pid, child_pids, &mean_send, procs);

        // Wait for all child procs to finish
        barrier();

        count = 0;
        // Get counts
        for(int i = 0; i < procs; i++){
			recv(&temp);
			send(parent_pid, child_pids[i], &count);
			wait();
			count += temp;
			// printf(1, "Got count %d from %d so total = %d\n", temp, i, total);
		}

		mean = 0.0;
		// Can parallelise this - barrier to ensure all means are done
		for (i = 0; i < N; i++){
			u[i][0] = u[i][N-1] = u[0][i] = 100.0;
			u[N-1][i] = 0.0;
			mean += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
		}
		mean /= (4.0 * N);
		printf(1, "Mean : ");
		printfloat(1, mean);
		printf(1, "\n");
		for (i = 1; i < N-1; i++ )
			for ( j= 1; j < N-1; j++) u[i][j] = mean;
		// Continue from where child procs left
		for(;;){
			diff = 0.0;
			for(i =1 ; i < N-1; i++)
				for(j =1 ; j < N-1; j++){
					w[i][j] = ( u[i-1][j] + u[i+1][j]+
							u[i][j-1] + u[i][j+1])/4.0;
					if( fabs(w[i][j] - u[i][j]) > diff )
						diff = fabs(w[i][j]- u[i][j]);
				count++;	
				}
			if(diff <= EPSILON) break;
			for (i =1; i< N-1; i++)	
				for (j =1; j< N-1; j++) u[i][j] = w[i][j];
		}
		for(i =0; i <N; i++){
			for(j = 0; j<N; j++){
				printfloat(1, u[i][j]);
				printf(1, ",");
			}
			printf(1,"\n");
		}
	}

	printf(1, "\nNumber of iteration: %d\n",count);

}
