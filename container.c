#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

char buf[512];

// MOD-3 : Helper function for ls
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}


// MOD-3 : ls for current directory
void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      if(!(fmtname(buf)[0] == 'c' && fmtname(buf)[1] == 'n' && fmtname(buf)[2] == 't'))
        printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

// MOD-3 : ls for container directory
void
ls_cnt(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      if(!(fmtname(buf)[0] == '.' || (fmtname(buf)[0] == '.' && fmtname(buf)[1] == '.')))
        printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

// MOD-3 : Container ls function
void
container_ls()
{
  int container = cid();
  char* name = (char*) malloc(sizeof(char)*5);
  name[0] = 'c'; name[1] = 'n'; name[2] = 't';
  name[3] = (container/10)+'0';
  name[4] = (container%10)+'0';
  ls(".");
  // printf(1, "ls done now doing ls in %s\n", name);
  if(cid() != -1)
    ls_cnt(name); 
}


// MOD-3 : Read and display contents of a file
// if not in my container then open host file
void
container_cat(char* filename)
{
  int container = cid();
  int n; int fd;
  char* name = (char*) malloc(sizeof(char)*(6+strlen(filename)));
  name[0] = 'c'; name[1] = 'n'; name[2] = 't';
  name[3] = (container/10)+'0';
  name[4] = (container%10)+'0';
  name[5] = '/';
  for(int i = 0; i < strlen(filename); i++)
    name[i+6] = filename[i];
  if(container == -1){
    fd = open(filename, 0);
    printf(1, "host file %s, %d\n", filename, fd);
  }
  else{
      fd = open(name, 0);
      if(fd < 0){
        fd = open(filename, 0);
      }
  }
  memset(buf, 0, 512);
  while((n = read(fd, buf, sizeof(buf))) > 0) {
       printf(1, "%s", buf);
  }
}


// MOD-3 : Create file directly in my container
int
container_create(char* filename)
{
  int container = cid();
  char* name = (char*) malloc(sizeof(char)*(6+strlen(filename)));
  name[0] = 'c'; name[1] = 'n'; name[2] = 't';
  name[3] = (container/10)+'0';
  name[4] = (container%10)+'0';
  if(open(name, 0) < 0)
    mkdir(name);
  name[5] = '/';
  for(int i = 0; i < strlen(filename); i++)
    name[i+6] = filename[i];
  return open(name, O_CREATE|O_RDWR);
}

// MOD-3 : When opening file first check if it exists in
// my container otherwise copy it from host
int
container_open(char*filename)
{
  int container = cid(); 
  if(container == -1)
    return open(filename, O_RDWR);
  int n;
  char* name = (char*) malloc(sizeof(char)*(6+strlen(filename)));
  name[0] = 'c'; name[1] = 'n'; name[2] = 't';
  name[3] = (container/10)+'0';
  name[4] = (container%10)+'0';
  if(open(name, 0) < 0)
    mkdir(name);
  name[5] = '/';
  for(int i = 0; i < strlen(filename); i++)
    name[i+6] = filename[i];
  int fd = open(name, O_RDWR);
  if(fd < 0){ // Copy file from host
    fd = open(name, O_CREATE|O_RDWR);
    int fd_host = open(filename, O_RDONLY);
    while((n = read(fd_host, buf, sizeof(buf))) > 0){
      write(fd, buf, n);
    }
  }
  else{ // Seek to end
    while((n = read(fd, buf, sizeof(buf))) > 0){}
  }
  return fd;
}

// MOD-3 : Write to the container file
int container_write(int fd, char* data)
{
  write(fd, data, strlen(data));
  return strlen(data);
}

// MOD-3 : Close container file
int container_close(int fd)
{
  return close(fd);
}

