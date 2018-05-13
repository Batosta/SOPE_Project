#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>	

int fdrequests;
int fdans;

void createOpenAnswerFIFO(pid_t pid);
void openRequestsFIFO();
void sendRequest();

int main(int argc, char *argv[]) {
   time_t initial=time(NULL);
   printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());
   
   if (argc!=4){
   	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
   	return -1;
   }
   if(argv[1] < 0){
	printf("time_out must be a value above 0\n");
	return -1;
   }
   if(argv[2] < 0){
	printf("num_wanted_seats must be a value above 0\n");
	return -1;
   }
   int time_out=atoi(argv[1]);

   createOpenAnswerFIFO(getpid());       //PARA LER a resposta. não para escrever
   /*verifica se obteve resposta*/
   while((time(NULL)-initial)<time_out){

   }

   openRequestsFIFO();

   sendRequest();
   
   return 0;
}

void openRequestsFIFO(){
    if((fdrequests = open("requests",O_WRONLY)) < 0){       // APPEND?

        printf("Error when opening requests FIFO\n");
        exit(1);
    }
}

void createOpenAnswerFIFO(pid_t pid){

   char str[3]="ans";
   char *end=str;
   end+=sprintf(end+3,"%ld",(long)pid);
   
   if(mkfifo(str, 0660) == -1){

	printf("Error when creating an answer FIFO\n");
	exit(1);
   }
   
   if((fdans = open(str,O_RDONLY)) < 0){

	printf("Error when opening an answer FIFO\n");
	exit(1);
   }

}

void sendRequest(){

}
