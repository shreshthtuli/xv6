#include "types.h"
#include "stat.h"
#include "user.h"

#define fabs(a) (a>=0) ? a : -a
#define N 10
#define EPSILON 0.01

int main(int argc, char *argv[])
{
	double diff;
	int i,j;
	double mean = 0.0;
	double u[N][N];
	double w[N][N];
	int count;
	int procs = 2;
	int proc_pids[procs];
	proc_pids[0] = getpid(); // Master proc
	int child_flag = 1;
	int pid = -1;
	int num;
	int prev = 0, next = 0;

	int start = uptime();

	// Initialise barrier
	barrier_init(procs + 1);

	// Initialise u matrix
	for (i = 0; i < N; i++){
		u[i][0] = u[i][N-1] = u[0][i] = 100.0;
		u[N-1][i] = 0.0;
		mean += u[i][0] + u[i][N-1] + u[0][i] + u[N-1][i];
	}

	// Set mean
	mean /= (4.0 * N);

	// Set u matrix
	for (i = 1; i < N-1; i++ )
		for ( j= 1; j < N-1; j++) u[i][j] = mean;

	// Initialise procs - master is num = 0
	for(int i = 1; i < procs; i++){
		num = i;
		pid = fork();
		if (pid != 0)
			proc_pids[i] = pid;
		else
			goto child;
		printf(1, "Init proc %d with pid %d\n", i, proc_pids[i]);
	}

	child_flag = 0;
	num = 0;
	dps();

	child:
	if(child_flag == 1){
		// Child process
		proc_pids[num] = getpid();
		prev = proc_pids[num - 1];
		if(num != procs-1)
			recv(&next);
	}
	else{
		// Master sends the next proc pid
		for(int i = 1; i < procs-1; i++){
			send(proc_pids[0], proc_pids[i], &proc_pids[i+1]);
		}
		// For master next = proc_pids[1]
		next = proc_pids[1];
	}

	double diff_next = 0, diff_prev = 0;

	// Parallelised jacobi method
	for(;;){
		diff = 0.0;
		// Compute jacobi for submatrix
		for(i = num*(N-2)/procs + 1; i < (num+1)*(N-2)/procs + 1; i++){
			for(j =1 ; j < N-1; j++){
				w[i][j] = ( u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1])/4.0;
				if( fabs(w[i][j] - u[i][j]) > diff )
					diff = fabs(w[i][j]- u[i][j]);
				count++;	
			}
		}
		// Share border results with next and prev
		if(num != procs-1){
			for(j = 1; j < N; j++)
				send(proc_pids[num], next, &u[(num+1)*(N-2)/procs][j]);
			send(proc_pids[num], next, &diff);
		}
		
		if(num != 0){
			for(j = 1; j < N; j++)
				recv(&u[num*(N-2)/procs][j]);
			recv(&diff_prev);
		}
		
		if(num != 0){
			for(j = 1; j < N; j++)
				send(proc_pids[num], prev, &u[num*(N-2)/procs + 1][j]);
			send(proc_pids[num], prev, &diff);
		}
		
		if(num != procs-1){
			for(j = 1; j < N; j++)
				recv(&u[(num+1)*(N-2)/procs + 1][j]);
			recv(&diff_next);
		}

		if(diff + diff_next + diff_prev <= EPSILON) break;
		for (i =1; i< N-1; i++)	
			for (j =1; j< N-1; j++) u[i][j] = w[i][j];
	}
	
	// Master prints the matrix
	if(num == 0){
		for(i = num*(N)/procs; i < (num+1)*(N)/procs; i++){
			for(j = 0; j<N; j++){
				printfloat(1, u[i][j]);
				printf(1, ",");
			}
			printf(1,"\n");
		}
		int temp = 0;
		for(int i = 1; i < procs; i++){
			send(proc_pids[0], proc_pids[i], &proc_pids[i+1]);
			recv(&temp);
			count += temp;
		}
		printf(1, "\nNumber of iteration: %d\n",count);
		printf(1, "Time = %d Ticks\n", uptime() - start); 

		for(int i = 0; i < procs - 1; i++)
			wait();
	}
	else{
		recv(&next);
		for(i = num*(N)/procs; i < (num+1)*(N)/procs; i++){
			for(j = 0; j<N; j++){
				printfloat(1, u[i][j]);
				printf(1, ",");
			}
			printf(1,"\n");
		}
		send(proc_pids[num], proc_pids[0], &count);
	}
	
	exit();
}
