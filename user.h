typedef void (* sig_handler) (char* msg);
struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
// MOD-1
int print_count(void);
int toggle(void);
int add(int, int);
int ps(void);
int dps(void); // Extra
int send(int, int, void*);
int recv(void*);
int send_multi(int, int*, void*, int);
int sigset(sig_handler);
int sigret(void);
int shutdown(void); // Extra
int start_timer(int); // Extra
int end_timer(int); // Extra
// MOD-2
int barrier_init(int);
int barrier(void);
// MOD-3
int create_container(void);
int destroy_container(int);
int join_container(int);
int leave_container(void);
int proc_stat_container(void);
int scheduler_log_on(void);
int scheduler_log_off(void);
int cid(void);
int memory_log_on(void);
int memory_log_off(void);
int memory_gva(int, int);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
void printfloat(int, float);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
// MOD-2
float atof(const char*);
void getline(char*, int*, int);
