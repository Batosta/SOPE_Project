#include "variables.h"

int REQUEST_FD;
int fdans;

void createOpenAnswerFIFO(pid_t pid);
void openRequestsFIFO();
void sendRequest();
int checkRequestConditions(char * argv[]);		//Faltam as condições -5 e -6

int main(int argc, char *argv[]) {

   time_t initial=time(NULL);
   printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());
   
   if (argc!=4){
   	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
   	return -1;
   }
   if(checkRequestConditions(argv) != 0){
	exit(1);
   }

   /*verifica se obteve resposta*/
   /*while((time(NULL)-initial)<time_out){

   }*/
   openRequestsFIFO();

   sendRequest(atoi(argv[2]),argv[3]);
   // createOpenAnswerFIFO(getpid());       //PARA LER a resposta. não para escrever

   printf("clientprint3\n");
   return 0;
}

void openRequestsFIFO(){
    if((REQUEST_FD = open("requests",O_WRONLY)) < 0){       // APPEND?

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

void sendRequest(int seats, char* seat_list){
    struct Request *request = malloc(sizeof(struct Request));
    request->pid = getpid();
    request->num_wanted_seats = seats;
    request->pref_seat_list = seat_list;
    write(REQUEST_FD, request, sizeof(struct Request));
}

int checkRequestConditions(char * argv[]) {

   if(atoi(argv[2]) >= MAX_CLI_SEATS) {
	printf("num_wanted_seats must be a value below MAX_CLI_SEATS.\n");
	return -1;
   }

   const char s[2] = " ";
   char *token;
   token = strtok(argv[3], s);
   int counter = 0;
   while(token != NULL) {
	
	if(atoi(token) > MAX_ROOM_SEATS || atoi(token) <= 0){

	   printf("All wanted seats must be values above 0 and below MAX_ROOM_SEATS.\n");
	   return -3;
	}
	
	token = strtok(NULL, s);
	counter++;
   }
   if(counter > MAX_CLI_SEATS || counter < atoi(argv[2])){
	printf("The wanted seats must have a size above num_wanted_seats and below MAX_CLI_SEATS.\n");
	return -2;
   }

   if(atoi(argv[1]) <= 0) {
	printf("time_out must be a value above 0.\n");
	return -4;
   }
   if(atoi(argv[2]) <= 0) {
	printf("num_wanted_seats must be a value above 0.\n");
	return -4;
   }

   return 0;
}






