#include "variables.h"

int REQUEST_FD;
int fdans;

void createOpenAnswerFIFO(pid_t pid);
void openRequestsFIFO();
void sendRequest();
void readAnswer();
int checkRequestConditions(char * argv[]);		//Faltam as condições -5 e -6

int main(int argc, char *argv[]) {

   time_t initial=time(NULL);
   printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

   if (argc!=4){
   	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
   	return -1;
   }

   openRequestsFIFO();

   sendRequest(atoi(argv[2]),argv[3],atoi(argv[1]));
   createOpenAnswerFIFO(getpid());       		//PARA LER a resposta. não para escrever
   readAnswer();

   return 0;
}

void openRequestsFIFO(){
    if((REQUEST_FD = open("requests",O_WRONLY)) < 0){       // APPEND?

        printf("Error when opening requests FIFO\n");
        exit(1);
    }
}

void createOpenAnswerFIFO(pid_t pid){

    char str[50];
    sprintf(str,"%s%ld", "ans", (long)pid);

   if(mkfifo(str, 0660) == -1){

	printf("Error when creating an answer FIFO\n");
	exit(1);
   }

   if((fdans = open(str,O_RDONLY)) < 0){

	printf("Error when opening an answer FIFO\n");
	exit(1);
   }
}

void sendRequest(int seats, char* seat_list, int time_out) {

    char[5000] str;
    sprintf(str, "%d %d %s\n", getpid(), seats, seat_list);
    write(REQUEST_FD, str, strlen(str));
}

void readAnswer() {

    char[1000] ans;
    
    int stat = read(fdans, &ans, 1000);

    if(stat < 0){
        printf("Error while reading.\n");
        return;
    }
}
