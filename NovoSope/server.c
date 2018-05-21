#include "server.h"


int REQUEST_FD;						//Request FIFO fd
int ANSWER_FD;						//Answer FIFO fd a answer tem haver com cada pid por isso não pode ser global
struct Seat * seats;					//Seats of the room
int nr_seats;						//Number of seats
pthread_mutex_t read_mut=PTHREAD_MUTEX_INITIALIZER;	//thread's mutex to read from buffer


struct Request * buffer=NULL;		//Unitary buffer. It's a Request Struct

int main(int argc, char *argv[]) {

	if (argc != 4){

   		printf("Usage: server <num_room_seats> <num_ticket_offices> <open_time>\n");
   		return -1;
	}

	nr_seats=atoi(argv[1]);

	seats=malloc(sizeof(struct Seat)*nr_seats);
	

	//cleanMessages();

	createRequestFIFO();
	for(unsigned int i = 1; i <= atoi(argv[2]); i++){
		
		createTicketOfficeThread(i);
	}

	openRequestFIFO();
	mainLoop();
	closeRequestFIFO();

	free(seats);
	
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
}

//Function that closes and destroys the request FIFO
void closeRequestFIFO(){

	close(REQUEST_FD);
	unlink("requests");
}

void mainLoop(){
	int n=1;
	while(n>0){
		if(buffer==NULL){
			readRequest();
		}
	n--;
	}
}

//Recebe struct. O array ainda vem com lixo. qual array?
//Function that receives and reads the request sent by a client through the "requests" FIFO
void readRequest(){
	struct Request req;
	if(read(REQUEST_FD, &req, sizeof(struct Request))>0)
		buffer=&req;	
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
void sendAnswer(const char* fifoName, struct Answer ans){
	write(ANSWER_FD, &ans, sizeof(struct Answer));
}

/*									THREADS										*/

//Function that creates a ticket office, which means it will create a new auxiliary thread that will process the requests
void createTicketOfficeThread(int id){

	printf("Created Ticket Office Thread.\n");
	
	pthread_t tid;
	pthread_create(&tid, NULL, ticketOfficeThread, NULL);			//2º NULL terá de ser alterado
}

//Ticket Office Thread, auxiliary thread
void * ticketOfficeThread(void *arg){

	printf("New thread.\n");
	struct Request req;
	while (1) {                             
		pthread_mutex_lock(&read_mut);      
		if (buffer != NULL) {
	    
			req=*buffer;
			printf("ticket");
			buffer=NULL;
			printf("bug");
			printf("%d\n",req.pid);
			pthread_mutex_unlock(&read_mut);
			processRequest(&req);
		}else	pthread_mutex_unlock(&read_mut);
    }
	return NULL;
}

//Process the request of the client
void processRequest(struct Request * req) {
	int err=testSomeCond(req);
	return;
	struct Answer ans;
	if(err<0){
		ans.error=err;
		//enviar resposta
		//escrever no ficheiro
		return;
	}
	int n_res=0;
	//while(n_res<=req->num_wanted_seats)
}

//Tests if the seat is free
int isSeatFree(int seatNum) {
	if(seats[seatNum].pid!=0)
		return 0;
	else
		return 1;
}

//Tests some of the conditions
int testSomeCond(struct Request * req){
	if(req->num_wanted_seats>MAX_CLI_SEATS){	//Caso em que excede o maximo por cliente
		printf("\n-1\n");
		return -1;
	}
	if(req->num_pref_seats<req->num_wanted_seats){	//Caso em que o numero de lugares preferidos é inferior aos wanted
		printf("\n-2\n");
		return -2;
	}
	int invalid_nr=0;	//counter for invalid preferred seats
	for(unsigned int i=0;i<req->num_pref_seats;i++){	//loop para calcular o numero de lugares preferidos invalidos
		if(req->pref_seat_list[i]<=0 || req->pref_seat_list[i]>nr_seats){
			invalid_nr++;
		}
	}
	if((req->num_pref_seats-invalid_nr)<req->num_wanted_seats){	//caso os validos sejam inferiores aos wanted
		printf("\n-3\n");
		return -3;
	}
	if(req->num_pref_seats<=0)	//caso os prefered seham 0
		return -4;
	int n=0;
	for(unsigned int i=0;i<nr_seats;i++){	//loop para calcular nr de lugares livres
		if(isSeatFree(i))
			n++;
	}
	if(n==0)	//caso esteja cheia a sala
		return -6;
	if(req->num_wanted_seats>n)	//caso nao haja lugares suficientes para o numero querido, isto nao quer dizer
		return -5;		//que os que ele quer estao livres
	return 0;
}

//Booking the seatNum
void bookSeat(int seatNum, int clientId){
	seats[seatNum].pid=clientId;
}

//Releases the seatNum
void freeSeat(int seatNum){
	seats[seatNum].pid=0;
}





