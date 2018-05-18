#include "client.h"


int REQUEST_FD;				//Request FIFO fd
int ANSWER_FD;				//Answer FIFO fd
FILE* FILE_POINTER;			//File Pointer for the answer fifos

int main(int argc, char *argv[]) {

	printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

	if (argc != 4){

	   	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
	   	return -1;
	}
	
	openRequestFIFO();
	sendRequest();	
}



/*									REQUESTS										*/

//Function that opens the clients's FIFO which is supposed to send the requests sent by the clients to the server
void openRequestFIFO(){

	if((REQUEST_FD = open("requests", O_WRONLY)) < 0){

		printf("Error while opening the request FIFO (client side).\n");
		exit(1);
	}
}

//void closeAnswerFIFO(const char* fifoName){ }

//Ainda nao envia a struct
//Function that sends through the "requests" FIFO a struct Request to the server
void sendRequest(){

    char str[5000];
    sprintf(str, "%d %d %s\n", getpid(), 2, "11 12 13");
    write(REQUEST_FD, str, strlen(str));
} 







/*									ANSWERS										*/

//Function that creates and opens the client's FIFO which is supposed to received the answer to the requests sent by the clients
void createOpenAnswerFIFO(){

	//pid_t clientPID = getpid();
	char str[10];
	sprintf(str,"%s%ld", "ans", (long)getpid());

	if((mkfifo(str, 0660)) != 0){
	
		printf("Error while creating the client's answer FIFO.\n");
		exit(1);
	}

	if((ANSWER_FD = open(str, O_RDONLY)) < 0){

		printf("Error while opening the client's answer FIFO (client side).\n");
		exit(1);
	}

	FILE_POINTER = fdopen(ANSWER_FD, "r");
}

//Function that receives through the answer fifo the answer of the server for the client requests
void readAnswer(){
	
	char in[200];
	read(ANSWER_FD, in, 200);
	printf("%s\n", in);
}
