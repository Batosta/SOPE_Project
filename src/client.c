#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

    if (argc!=4){
	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
	return -1;
   }
   char str[3]="ans";
   char *end=str;
   int fd;
   end+=sprintf(end+3,"%ld",(long)getpid());
   mkfifo(str,0660);

   mkfifo("requests",0660);
   fd=open("requests",O_WRONLY);
   write(fd,
  

  return 0;
}
