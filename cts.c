#include "container.c"

#define procs 3
#define num_containers 2

int parent_pid;
int num;


int main(int argc, char *argv[])
{   
    container_ls();
    int host = open("filehost", O_CREATE|O_RDWR);
    write(host, "This is a host file", 19);
    close(host);
    int id = create_container();
    join_container(id);
    // scheduler_log_on();
    printf(1, "Joined container %d\n", id);
    proc_stat_container();
    dps();

    int fd = container_create("file");
    container_write(fd, "Modified");
    container_close(fd);
    container_cat("file");
    printf(1, "Done\n");
    container_ls();
    container_cat("file");
    sleep(100);
    printf(1, "\n");
    fd = container_open("file");
    container_write(fd, "Modifiedagain");
    container_close(fd);
    container_cat("file");
    printf(1, "\n");
    fd = container_open("filehost");
    container_write(fd, "ModifiedHostfile\n");
    container_close(fd);
    container_cat("filehost");
    container_ls();
    leave_container();
    printf(1, "Containerid = %d\n", cid());
    container_ls();
    container_cat("filehost");
    printf(1, "\n");
    return 0;
}