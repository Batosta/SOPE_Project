#include "server.h"


int REQUEST_FD;				//Request FIFO fd
int ANSWER_FD;				//Answer FIFO fd a answer tem haver com cada pid por isso não pode ser global
FILE* FILE_POINTER;			//File Pointer for the request fifo
int * Seats;


struct Request * buffer;		//Unitary buffer. It's a Request Struct

int main(int argc, char *argv[]) {

	if (argc != 4){

   		printf("Usage: server <num_room_seats> <num_ticket_offices> <open_time>\n");
   		return -1;
	}

	Seats=malloc(sizeof(int)*atoi(argv[1]));

	//cleanMessages();

	createRequestFIFO();
	openRequestFIFO();
	readRequest();
	closeRequestFIFO();

	for(unsigned int i = 1; i <= atoi(argv[2]); i++){
		
		createTicketOfficeThread(i);
	}

	free(Seats);
}

/*									REQUESTS										*/

//Function that creates the server's FIFO which is supposed to received the requests sent by the clients
void createRequestFIFO(){

	if((mkfifo("requests", 0660)) != 0){
		
		printf("Error while creating the request FIFO.\n");
		exit(1);
	}
}

//Function that opens the server's FIFO which is supposed to received the requests sent by the clients
void openRequestFIFO(){

	if((REQUEST_FD = open("requests", O_RDONLY)) < 0){

		printf("Error while opening the request FIFO (server side).\n");
		exit(1);
	}

	FILE_POINTER = fdopen(REQUEST_FD, "r");
}

//Function that closes and destroys the request FIFO
void closeRequestFIFO(){

	close(REQUEST_FD);
	unlink("requests");
}

//Recebe struct. Esta parte nao esta a ler o array direito nao sei porque.
//Function that receives and reads the request sent by a client through the "requests" FIFO
void readRequest(){
	struct Request req;
	read(REQUEST_FD, &req, sizeof(struct Request));
	printf("%d %d\n", req.pid,req.num_wanted_seats);
	for(int i = 0;i<1 ; i++){
		printf("%d; ", req.pref_seat_list[i]);
	}	
}




/*									ANSWERS										*/

//Function that opens the FIFO fifoName which is supposed to transmit the answers sent by the server to the clients
void openAnswerFIFO(const char* fifoName){

	if((ANSWER_FD = open(fifoName, O_WRONLY)) < 0){

		printf("Error while opening the answer FIFO (server side).\n");
		exit(1);
	}
}

//Function that sends through the answer FIFO the answer of the server to the client that requested seats
void sendAnswer(const char* fifoName, const char* message){

	char str[5000];
	sprintf(str, "%d %d %s\n", getpid(), 2, "11 12 13");
	write(ANSWER_FD, str, strlen(str));
}






/*									THREADS										*/

//Function that creates a ticket office, which means it will create a new auxiliary thread that will process the requests
void createTicketOfficeThread(int id){

	printf("Created Ticket Office Thread.\n");
	
	pthread_t tid;
	pthread_create(&tid, NULL, ticketOfficeThread, NULL);			//2º NULL terá de ser alterado
	//pthread_join(tid, NULL);						//idk se aqui isto está fixe,acho que não.
}

//Ticket Office Thread, auxiliary thread
void * ticketOfficeThread(void *arg){

	printf("New thread.\n");
	return NULL;
}

//Tests if the seat is free
int isSeatFree(int * seats,int seatNum) {
	if(seats[seatNum]!=0)
		return 0;
	else
		return 1;
}

//Booking the seatNum
void bookSeat(int * seats, int seatNum, int clientId){
	seats[seatNum]=clientId;
}

//Releases the seatNum
void freeSeat(int * seats, int seatNum){
	seats[seatNum]=0;
}





