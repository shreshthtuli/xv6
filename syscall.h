// System call numbers
#define SYS_fork        1
#define SYS_exit        2
#define SYS_wait        3
#define SYS_pipe        4
#define SYS_read        5
#define SYS_kill        6
#define SYS_exec        7
#define SYS_fstat       8
#define SYS_chdir       9
#define SYS_dup         10
#define SYS_getpid      11
#define SYS_sbrk        12
#define SYS_sleep       13
#define SYS_uptime      14
#define SYS_open        15
#define SYS_write       16
#define SYS_mknod       17
#define SYS_unlink      18
#define SYS_link        19
#define SYS_mkdir       20
#define SYS_close       21
// MOD-1 : Additional syscalls
#define SYS_print_count 22
#define SYS_toggle      23
#define SYS_add         24
#define SYS_ps          25
#define SYS_dps         26
#define SYS_send        27
#define SYS_recv        28
#define SYS_send_multi  29
#define SYS_sigset      30
#define SYS_sigret      31
#define SYS_shutdown    32
#define SYS_start_timer 33
#define SYS_end_timer   34
// MOD-2 : Barrier syscalls
#define SYS_barrier_init        35
#define SYS_barrier     36
// MOD-3 : Container syscalls
#define SYS_create_container    37
#define SYS_destroy_container   38
#define SYS_join_container      39
#define SYS_leave_container     40
#define SYS_proc_stat_container 41
#define SYS_scheduler_log_on    42
#define SYS_scheduler_log_off   43
#define SYS_cid         44
#define SYS_memory_log_on       45
#define SYS_memory_log_off      46
#define SYS_memory_gva          47