#include "client.h"


int REQUEST_FD;				//Request FIFO fd
int ANSWER_FD;				//Answer FIFO fd
struct Answer ans;
char answer_name[10];

void handle_alarm( int sig ) {
	ans.error=-7;
	handleAnswer();
}

int main(int argc, char *argv[]) {
	signal(SIGALRM, handle_alarm);
	printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

	if (argc != 4){

	   	printf("Usage: client <time_out> <num_wanted_seats> <pref_seat_list>\n");
	   	return -1;
	}
	if(atoi(argv[1])<=0)
		printf("Time_out needs to be over 0");
	alarm(atoi(argv[1]));
	createAnswerFIFO();
	
	openRequestFIFO();

//Creates the struct to send through the request FIFO
	struct Request req;
	req.pid=(int)getpid();
	req.num_wanted_seats = atoi(argv[2]);
	char *token;
	int i = 0;
	token = strtok(argv[3], " ");
	while(token != NULL){
		req.pref_seat_list[i] = atoi(token);
		i++;
		token = strtok(NULL," ");
	}
	req.num_pref_seats=i;

	sendRequest(req);
	
	openAnswerFIFO();	//isto tem de estar comentado enquanto nao houver answer
	readAnswer();		//isto tem de estar comentado enquanto nao houver answer
	handleAnswer();
	closeAnswerFIFO();
	
}



/*									REQUESTS										*/

//Function that opens the clients's FIFO which is supposed to send the requests sent by the clients to the server
void openRequestFIFO(){

	if((REQUEST_FD = open("requests", O_WRONLY|O_APPEND)) < 0){

		printf("Error while opening the request FIFO (client side).\n");
		exit(1);
	}
}


//Function that sends through the "requests" FIFO a struct Request to the server
void sendRequest(struct Request r){
    write(REQUEST_FD, &r, sizeof(struct Request));
    close(REQUEST_FD);
} 


/*									ANSWERS										*/

//Function that creates client's FIFO which is supposed to received the answer to the requests sent by the clients
void createAnswerFIFO(){
	sprintf(answer_name,"%s%ld", "ans", (long)getpid());

	if((mkfifo(answer_name, 0660)) != 0){
	
		printf("Error while creating the client's answer FIFO.\n");
		exit(1);
	}
}

//Opens the answerFIFO
void openAnswerFIFO(){
	printf("%s",answer_name);
	if((ANSWER_FD = open(answer_name, O_RDONLY)) < 0){

		printf("Error while opening the client's answer FIFO (client side).\n");
		exit(1);
	}
}

//Closes and destroys the answer FIFO
void closeAnswerFIFO(){
	close(ANSWER_FD);
	unlink(answer_name);
}
	

//Function that receives through the answer fifo the answer of the server for the client requests
void readAnswer(){
	if(read(ANSWER_FD, &ans, sizeof(ans))<0)
		printf("Error when trying to read the client's answer FIFO (client side).\n");
}

//Function that handles the answer recieved
void handleAnswer() {
	if(ans.error==0)
		writeBook();
	writeLog();
}	


//Function that writes in the clog.txt
void writeLog(){
	FILE * log=fopen("clog.txt","a");
	if(log==NULL){
		printf("Error Opening the clog.txt file (Client side)");
		return;
	}
	//Case of success 
	if(ans.error==0){
		for(int i=0;i<ans.num_reserved;i++){
			char message[WIDTH_PID+1+WIDTH_SEAT+1+WIDTH_XXNN];
			//write in message array 
			sprintf(message,"%0*d %0*d.%0*d %0*d",WIDTH_PID,(WIDTH_XXNN-1)/2,(WIDTH_XXNN-1)/2,WIDTH_SEAT,(int)getpid(),i+1,ans.num_reserved,ans.res_list[i]);
			writeMessage(log, message);
		}
	}else{	//case of error
		char message[WIDTH_PID+4];
		char * error=errorToChar(ans.error);
		sprintf(message,"%0*d %s",WIDTH_PID,(int)getpid(),error);
		writeMessage(log, message);
	}
}



//Function that writes in cbook.txt
void writeBook(){
	FILE * book=fopen("cbook.txt","a");
	if(book==NULL){
		printf("Error Opening the cbook.txt file (Client side)");
		return;
	}
	for(int i=0;i<ans.num_reserved;i++){
		char message[WIDTH_SEAT];
		sprintf(message,"%0*d",WIDTH_SEAT,ans.res_list[i]);
		writeMessage(book, message);
	}
}
	









