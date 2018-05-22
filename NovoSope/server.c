#include "server.h"


int REQUEST_FD;						//Request FIFO fd
struct Seat * seats;					//Array of seats of the room
int nr_seats;						//Number of seats
pthread_mutex_t read_mut = PTHREAD_MUTEX_INITIALIZER;	//thread's mutex to read from buffer

FILE * slog;
int num_ticket_offices;
pthread_t * t_tid;
int alrm;

struct Request * buffer=NULL;				//Unitary buffer. It's a Request Struct

void handle_alarm(int sig) {
	alrm=1;
}

int main(int argc, char *argv[]) {
	alrm=0;
	signal(SIGALRM, handle_alarm);

	if (argc != 4){

   		printf("Usage: server <num_room_seats> <num_ticket_offices> <open_time>\n");
   		return -1;
	}

	alarm(atoi(argv[3]));
	num_ticket_offices = atoi(argv[2]);

	nr_seats=atoi(argv[1]);

	seats=malloc(sizeof(struct Seat)*nr_seats);
	t_tid=malloc(sizeof(pthread_t)*num_ticket_offices);

	cleanMessages();

	createRequestFIFO();
	createTicketOfficeThread(atoi(argv[2]));

	openRequestFIFO();
	mainLoop();
	closeRequestFIFO();
	
	for(int i=0;i<num_ticket_offices;i++){
		char message[9];
		pthread_join(t_tid[i],NULL);
		sprintf(message,"%0*d-CLOSE",2,(i+1));
		writeMessage(slog,message);
	}
	char message_s[]="SERVER CLOSED";
	writeMessage(slog,message_s);
	//close slog
	free(t_tid);
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

	while(!alrm){
		if(buffer==NULL){
			readRequest();
		}
	sleep(1);
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
void openAnswerFIFO(int pid, struct Answer * ans){

	char answer_name[10];
	sprintf(answer_name, "%s%ld", "ans", (long)pid);

	int ANSWER_FD;

	if((ANSWER_FD = open(answer_name, O_WRONLY)) < 0){

		printf("Error while opening the answer FIFO %s (server side).\n", answer_name);
       		return;
	}

	sendAnswer(ANSWER_FD, ans);
	close(ANSWER_FD);
}

//Function that sends through the answer FIFO the answer of the server to the client that requested seats
void sendAnswer(int ANSWER_FD, struct Answer * ans){

	write(ANSWER_FD, ans, sizeof(struct Answer));
}

/*									THREADS										*/

//Function that creates a ticket office, which means it will create a new auxiliary thread that will process the requests
void createTicketOfficeThread(int ticketOfficesNumber){				//id vai ser usado para escrever ID-OPEN ou ID-CLOSE
	int thrarg[ticketOfficesNumber];

	openLog();
	for(int i = 0; i < ticketOfficesNumber; i++){
		
		char message[6];
		printf("Created Ticket Office Thread.\n");
		thrarg[i] = i + 1;
		sprintf(message,"%0*d-OPEN",2,thrarg[i]);
		writeMessage(slog,message);
		pthread_t tid;
		pthread_create(&tid, NULL, ticketOfficeThread, &thrarg[i]);
		t_tid[i]=tid;
		sleep(1);
	}
}

//Functions 
void openLog(){

	slog = fopen("slog.txt", "a");
	if(slog == NULL){
		
		printf("Error Opening the slog.txt file (Server side)");
		return;
	}
}

//Ticket Office Thread, auxiliary thread
void * ticketOfficeThread(void *arg){

	printf("New thread.\n");
	int id = *(int *)arg;	
	printf("id = %d", id);
	struct Request req;
	while (!alrm) {                             
		pthread_mutex_lock(&read_mut);
		if (buffer != NULL) {
			req=*buffer;
			buffer=NULL;
			pthread_mutex_unlock(&read_mut);
			processRequest(&req, id);
		}else	
			pthread_mutex_unlock(&read_mut);
   	}
	return NULL;
}


//Process the request of the client
void processRequest(struct Request * req, int id) {

	printf("Procces nr %d with %d places and %d prefered\n",req->pid,req->num_wanted_seats,req->num_pref_seats);

	struct Answer * ans = malloc(sizeof(struct Answer));
	ans->error = testSomeCond(req);
	ans->req.pid = req->pid;
	ans->req.num_wanted_seats = req->num_wanted_seats;
	ans->req.num_pref_seats = req->num_pref_seats;
	ans->id = id;
	for(int k = 0; k < MAX_ROOM_SEATS; k++){
		ans->req.pref_seat_list[k] = req->pref_seat_list[k];
	}
	

	if(ans->error != 0){					//If error

		openAnswerFIFO(req->pid, ans);
		handleAnswer(ans);
		return;
	} else{
		
		int n_reserved = 0;
		int n_tried = 0;
		while(n_tried < req->num_pref_seats && n_reserved < req->num_wanted_seats){

			pthread_mutex_lock(&seats[req->pref_seat_list[n_tried] - 1].seat_mut);
			if(isSeatFree(seats, req->pref_seat_list[n_tried] - 1)){				//Checkar o bookSeats
				
				bookSeat(seats, req->pref_seat_list[n_tried] - 1, req->pid);
				DELAY();
				ans->res_list[n_reserved] = req->pref_seat_list[n_tried];
				n_reserved++;
			}

			pthread_mutex_unlock(&seats[req->pref_seat_list[n_tried] - 1].seat_mut);
			n_tried++;
		}

		ans->num_reserved = n_reserved;
		
		if(ans->num_reserved < req->num_wanted_seats){		//Ainda nao testado

			ans->error = -5;
			
			for(int k = 0; k < ans->num_reserved; k++){

				freeSeat(seats, ans->res_list[k] - 1);
			}
		}	
	}

	openAnswerFIFO(req->pid, ans);
	handleAnswer(ans);
	free(ans);
}


//Tests some of the conditions
int testSomeCond(struct Request * req){

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


//Function that handles the answer recieved
void handleAnswer(struct Answer * ans) {
	
	if(ans->error == 0)
		writeBook();
	writeLog(ans);
}

//Function that writes in the slog.txt
void writeLog(struct Answer * ans){

	//Case of success
	if(ans->error == 0){
		
		char mainMessage[WIDTH_PID + 8 + MAX_CLI_SEATS*(WIDTH_SEAT+1) + 2 + ans->num_reserved*(WIDTH_SEAT+1)];
		char message1[WIDTH_PID + 8];
		char message2[WIDTH_SEAT+1];
		char message3[2];
		char message4[WIDTH_SEAT+1];


		sprintf(message1, "%0*d-%0*d-%0*d: ", 2, ans->id, WIDTH_PID, ans->req.pid, 2, ans->req.num_wanted_seats);
		strcat(mainMessage, message1);
		for(int k = 0; k < ans->req.num_pref_seats; k++){

			sprintf(message2, "%0*d ", WIDTH_SEAT, ans->req.pref_seat_list[k]);
			strcat(mainMessage, message2);
		}
		sprintf(message3, "- ");
		strcat(mainMessage, message3);
		for(int k = 0; k < ans->num_reserved; k++){
			
			
			sprintf(message4, "%0*d ", WIDTH_SEAT, ans->res_list[k]);
			strcat(mainMessage, message4);
		}

		writeMessage(slog, mainMessage);
	} else {										//Case of error

		char mainMessage[WIDTH_PID + 8 + MAX_CLI_SEATS*(WIDTH_SEAT+1) + 2 + ans->num_reserved*(WIDTH_SEAT+1)];
		char message1[WIDTH_PID + 8];
		char message2[MAX_CLI_SEATS*(WIDTH_SEAT+1)];
		char message3[2];
		char message4[3];

		sprintf(message1, "00-%0*d-%0*d: ", WIDTH_PID, ans->req.pid, 2, ans->req.num_wanted_seats);
		strcat(mainMessage, message1);
		for(int k = 0; k < ans->req.num_pref_seats; k++){

			sprintf(message2, "%0*d ", WIDTH_SEAT, ans->req.pref_seat_list[k]);
			strcat(mainMessage, message2);
		}
		sprintf(message3, "- ");
		strcat(mainMessage, message3);

		char * error = errorToChar(ans->error);
		sprintf(message4, "%s", error);
		strcat(mainMessage, message4);

		writeMessage(slog, mainMessage);
	}
}

//Function that writes in sbook.txt
void writeBook(){
	
	FILE * book=fopen("sbook.txt","a");
	if(book == NULL){
		printf("Error Opening the sbook.txt file (Server side)");
		return;
	}

	for(int i = 0; i < nr_seats; i++){
		
		if(seats[i].pid != 0){
			
			sleep(0);
			char message[WIDTH_SEAT];
			sprintf(message,"%0*d",WIDTH_SEAT, i+1);
			writeMessage(book, message);
		}
	}
	fclose(book);
}
