#include "variables.h"

pthread_mutex_t mutex;

void *ticket_office_thr_func(void *arg);
void openRequestFIFO();
void *main_thr_func(void *arg);
void createTicketOfficeThread(int id);
void createRequestFIFO();
int checkRequestConditions(struct Seat * seats, struct Request request);
void tryToReadRequest(char * str);
void processRequest(struct Request request, struct Seat * seats);
void createMainThread();
int openAnsFIFO(pid_t pid);
struct Request readRequestString(char * str);

int isSeatFree(struct Seat *seats, int seatNum);
void bookSeat(struct Seat *seats, int seatNum, int clientId);
void freeSeat(struct Seat *seats, int seatNum);

int REQUEST_FD;
FILE* FILE_POINTER;
pthread_t main_thread_tid;

char * buffer = NULL;

pthread_mutex_t mutex;
pthread_mutex_t mutexTicketOffice;

int main(int argc, char *argv[]) {
   

   if (argc!=4){
   	printf("Usage: server <num_room_seats> <num_ticket_offices> <open_time>\n");
   	return -1;
   }
   if(atoi(argv[1]) <= 0){
	printf("num_room_seats must be a value above 0\n");
	return -1;
   }
   if(atoi(argv[2]) <= 0){
	printf("num_ticket_offices must be a value above 0\n");
	return -1;
   }
   if(atoi(argv[3]) <= 0){
	printf("open_time must be a value above 0\n");
	return -1;
   }
   
   int nSeats = atoi(argv[1]);
   for(int i = 1; i <= nSeats; i++){
	
	allSeats[i-1].pid = -1;                   // lugar livre se pid == -1
   }
   createRequestFIFO();
   openRequestFIFO();
   createMainThread();

   int n=atoi(argv[2]);
   for(int i = 1; i < n; i++){
	createTicketOfficeThread(i);
   }

    pthread_join(main_thread_tid,NULL);

    unlink("requests");
}

void createRequestFIFO(){
   mkfifo("requests", 0660);
}

void openRequestFIFO(){
   
   if((REQUEST_FD = open("requests",O_RDONLY)) < 0) {
	printf("Error when opening the request FIFO\n");
	exit(1);
   }
   FILE_POINTER = fdopen(REQUEST_FD, "r");
}

void tryToReadRequest(char * str){

   size_t len = 0;

   getline(&str, &len, FILE_POINTER);
}


void createMainThread(){

   pthread_create(&main_thread_tid, NULL, main_thr_func, NULL);
}


void createTicketOfficeThread(int id){

   pthread_t tid;

   //writeNumber("slog.txt", id);
   //writeMessage("slog.txt", "-OPEN\n");
   pthread_create(&tid, NULL, ticket_office_thr_func, allSeats);
   //writeNumber("slog.txt", id);
   //writeMessage("slog.txt", "-CLOSED\n");
   
   //pthread_join(tid, NULL);
}

void *ticket_office_thr_func(void *arg) {

    printf("New Ticket Office Called\n");
    
    struct Request req;
    struct Seat *seats = arg;

    while (1) {                             //    enquanto o tempo da thread n acabar
	pthread_mutex_lock(&mutexTicketOffice);      
	if (buffer != NULL) {
	    
	    req=readRequestString(buffer);
            buffer=NULL;
	    pthread_mutex_unlock(&mutexTicketOffice);
            processRequest(req,seats);
        }else{
	    pthread_mutex_unlock(&mutexTicketOffice);
	}
    }

   pthread_exit(NULL);
}

void processRequest(struct Request request, struct Seat * seats){

    int stat;
    int fdans;

    fdans = openAnsFIFO(request.pid);

    if ((stat = checkRequestConditions(seats, request)) != 0) {
        write(fdans, &stat, sizeof(stat));
	//writeMessage("slog.txt", "TO-");
        //writeNumber("slog.txt", request.pid);
        //writeMessage("slog.txt", "-");
	//writeNumber("slog.txt", request.num_wanted_seats);	
	return;
    }

    write(fdans, &request.num_wanted_seats, sizeof(int));
    for (int i = 0; i < request.num_wanted_seats; i++) {

        write(fdans, &request.pref_seat_list[i], sizeof(int));
    }
}

void *main_thr_func(void *arg){

    char * str = NULL;
    
    printf("Main Thread Called\n");

	while(1){                //LER REQUEST
	  if(buffer==NULL){
          tryToReadRequest(str);
          buffer = str;
	  }
	}
    pthread_exit(NULL);
}

int isSeatFree(struct Seat * seats, int seatNum){
    return seats[seatNum-1].pid == -1;
}

void bookSeat(struct Seat * seats, int seatNum, pid_t clientId){
    seats[seatNum-1].pid = clientId;
}

void freeSeat(struct Seat * seats, int seatNum){
    seats[seatNum-1].pid = -1;
}

int checkRequestConditions(struct Seat * seats, struct Request request) {

    int lugarocupado = 0;
    int lugar = -1;

    int counter = 0;

    if(request.num_wanted_seats >= MAX_CLI_SEATS) {
        printf("num_wanted_seats must be a value below MAX_CLI_SEATS.\n");
        return -1;
    }

    lugar = request.pref_seat_list[0];

    for( int i = 1 ; lugar != 0 ; i++) {

        if(lugar > MAX_ROOM_SEATS || lugar <= 0) {
            printf("All wanted seats must be values above 0 and below MAX_ROOM_SEATS%d.\n",lugar);
            return -3;
        }

        if(isSeatFree(seats,lugar)) {
            bookSeat(seats, lugar, request.pid);
        } else {
            lugarocupado = 1;
            break;
        }

        lugar = request.pref_seat_list[i];
        counter++;
    }

    if(counter > MAX_CLI_SEATS || counter < request.num_wanted_seats){
        printf("The wanted seats must have a size above num_wanted_seats and below MAX_CLI_SEATS.\n");
        return -2;
    }

    if(request.time_out <= 0) {
        printf("time_out must be a value above 0.\n");
        return -4;
    }
    if(request.num_wanted_seats <= 0) {
        printf("num_wanted_seats must be a value above 0.\n");
        return -4;
    }

    if (lugarocupado == 1) {
        for( int i = 0 ; lugar != 0 ; i++) {
            lugar = request.pref_seat_list[i];
            freeSeat(seats, lugar);
        }
    }

    return 0;
}

int openAnsFIFO(pid_t pid){
    int fd;
    char str[50];
    sprintf(str,"%s%ld", "ans", (long)pid);

    fd=open(str,O_WRONLY);

    return fd;
}

struct Request readRequestString(char * str){

   struct Request r;
   char * pref = NULL;
   
   sscanf(str, "%d %d %s\n", &r.pid, &r.num_wanted_seats, pref);

   const char s[2] = " ";
   char *lugar;
   int i = 0;
   lugar = strtok(pref, s);

   while (lugar != NULL) {
       r.pref_seat_list[i] = atoi(lugar);
       lugar = strtok(NULL, s);
       i++;
   }

   return r;
}




