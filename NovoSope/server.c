#include "server.h"


int REQUEST_FD;						//Request FIFO fd
struct Seat * seats;					//Array of seats of the room
int nr_seats;						//Number of seats
pthread_mutex_t read_mut = PTHREAD_MUTEX_INITIALIZER;	//thread's mutex to read from buffer


struct Request * buffer=NULL;				//Unitary buffer. It's a Request Struct

int main(int argc, char *argv[]) {

	if (argc != 4){

   		printf("Usage: server <num_room_seats> <num_ticket_offices> <open_time>\n");
   		return -1;
	}

	nr_seats=atoi(argv[1]);

	seats=malloc(sizeof(struct Seat)*nr_seats);

	cleanMessages();

	createRequestFIFO();
	createTicketOfficeThread(atoi(argv[2]));

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
	int n=10000;
	while(1){
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
void openAnswerFIFO(int pid, struct Answer ans){

	char answer_name[10];
	sprintf(answer_name, "%s%ld", "ans", (long)pid);

	int ANSWER_FD;

	if((ANSWER_FD = open(answer_name, O_WRONLY)) < 0){

		printf("Error while opening the answer FIFO (server side).\n");
		exit(1);
	}

	sendAnswer(ANSWER_FD, ans);
}

//Function that sends through the answer FIFO the answer of the server to the client that requested seats
void sendAnswer(int ANSWER_FD, struct Answer ans){

	write(ANSWER_FD, &ans, sizeof(struct Answer));
}

/*									THREADS										*/

//Function that creates a ticket office, which means it will create a new auxiliary thread that will process the requests
void createTicketOfficeThread(int ticketOfficesNumber){				//id vai ser usado para escrever ID-OPEN ou ID-CLOSE

	for(int i = 0; i < ticketOfficesNumber; i++){

		printf("Created Ticket Office Thread.\n");

		pthread_t tid;
		pthread_create(&tid, NULL, ticketOfficeThread, (i+1));			//2º NULL terá de ser alterado

	}
}

//Ticket Office Thread, auxiliary thread
void * ticketOfficeThread(void *arg){

	printf("New thread.\n");
	struct Request req;
	while (1) {                             
		pthread_mutex_lock(&read_mut);
		if (buffer != NULL) {
	    
			req=*buffer;
			buffer=NULL;
			pthread_mutex_unlock(&read_mut);
			processRequest(&req);
		}else	pthread_mutex_unlock(&read_mut);
    }
	return NULL;
}


//Process the request of the client
void processRequest(struct Request * req) {

	int err = testSomeCond(req);

	struct Answer ans;
	if(err != 0){					//If error

		ans.error = err;
		//enviar resposta
		//escrever no ficheiro
		return;
	} else{
		
		int n_reserved = 0;
		int n_tried = 0;
		while(n_tried < req->num_pref_seats && n_reserved < req->num_wanted_seats){

			if(isSeatFree(seats, req->pref_seat_list[n_tried] - 1)){				//Checkar o bookSeats
				
				pthread_mutex_lock(&seats[req->pref_seat_list[n_tried] - 1].seat_mut);
				bookSeat(seats, req->pref_seat_list[n_tried] - 1, req->pid);
				DELAY();
				ans.res_list[n_reserved] = req->pref_seat_list[n_tried];
				ans.num_reserved++;
				n_reserved++;
				pthread_mutex_unlock(&seats[req->pref_seat_list[n_tried] - 1].seat_mut);
			}
			n_tried++;

		}
		
		if(n_reserved != req->num_wanted_seats){		//Ainda nao testado

			ans.error = -5;
			
			for(int k = 0; k < nr_seats; k++){

				if(seats[k].pid == req->pid){

					freeSeat(seats, k);
				}
			}
		}
	}

	//sendAnswer
	//sendAnswer("fifoname", );
	//escrever nos files

	openAnswerFIFO(req->pid, ans);
	handleAnswer(&ans);
}


//Tests some of the conditions
int testSomeCond(struct Request * req){
	printf("\n%d %d\n",req->num_pref_seats,req->num_wanted_seats);
	if(req->num_wanted_seats>MAX_CLI_SEATS){	//Caso em que excede o maximo por cliente
		return -1;
	}
	if(req->num_pref_seats < req->num_wanted_seats){	//Caso em que o numero de lugares preferidos é inferior aos wanted
		return -2;
	}
	for(int i=0;i<req->num_pref_seats;i++){	//loop para calcular o numero de lugares preferidos invalidos
		if(req->pref_seat_list[i]<=0 || req->pref_seat_list[i]>nr_seats){
			return -3;
		}
	}
	if(req->num_pref_seats<=0){	//caso os prefered sejam 0
		return -4;
	}
	int n=0;
	for(int i=0;i<nr_seats;i++){	//loop para calcular nr de lugares livres
		if(isSeatFree(seats, i))
			n++;
	}
	if(n==0){	//caso esteja cheia a sala
		return -6;
	}
	if(req->num_wanted_seats>n){	//caso nao haja lugares suficientes para o numero querido, isto nao quer dizer
		return -5;		//que os que ele quer estao livres
	}
	return 0;
}

//Tests if the seat is free
int isSeatFree(struct Seat *seats, int seatNum) {

	if(seats[seatNum].pid != 0)		//Not free seat
		return 0;
	else					//Free seat
		return 1;
}

//Booking the seatNum
void bookSeat(struct Seat *seats, int seatNum, int clientId){

	seats[seatNum].pid = clientId;
	
}

//Releases the seatNum
void freeSeat(struct Seat *seats, int seatNum){

	seats[seatNum].pid = 0;
}


void handleAnswer(struct Answer * ans) {
	
	if(ans->error == 0)
		writeBook();
	writeLog();
}

void writeBook(){

}

void writeLog(){

	
}
