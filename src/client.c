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

   /*verifica se obteve resposta*/
   /*while((time(NULL)-initial)<time_out){

   }*/
   openRequestsFIFO();

   sendRequest(atoi(argv[2]),argv[3],atoi(argv[1]));
   createOpenAnswerFIFO(getpid());       //PARA LER a resposta. não para escrever
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

   printf("Antes\n");
   if((fdans = open(str,O_RDONLY)) < 0){

	printf("Error when opening an answer FIFO\n");
	exit(1);
   }
    printf("Depois\n");

}

void sendRequest(int seats, char* seat_list, int time_out) {
    struct Request *request = malloc(sizeof(struct Request));
    request->pid = getpid();
    request->num_wanted_seats = seats;

    const char s[2] = " ";
    char *lugar;
    int i = 0;
    lugar = strtok(seat_list, s);

    while (lugar != NULL) {
        request->pref_seat_list[i] = atoi(lugar);
        lugar = strtok(NULL, s);
        i++;
    }

    request->time_out = time_out;
    write(REQUEST_FD, request, sizeof(struct Request));
}

void readAnswer() {
    //char answer[MAX_CLI_SEATS+1];
    int ans;
    while (1) {
        if(read(fdans, &ans, sizeof(int))>0)
            break;
    }
    if(ans < 0){
        printf("answer: %d\n", ans);
        return;
    }

}
