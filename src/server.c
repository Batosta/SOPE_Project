#include "variables.h"

pthread_mutex_t mutex;

void *ticket_office_thr_func(void *arg);
void openRequestFIFO();
void *main_thr_func(void *arg);
void createTicketOfficeThread();
void createRequestFIFO();
int checkRequestConditions(struct Seat * seats, struct Request request);
struct Request tryToReadRequest();
void printRequest(struct Request r);
void createMainThread();
int openAnsFIFO(pid_t pid);

int isSeatFree(struct Seat *seats, int seatNum);
void bookSeat(struct Seat *seats, int seatNum, int clientId);
void freeSeat(struct Seat *seats, int seatNum);

int REQUEST_FD;
pthread_t main_thread_tid;
struct Request * buffer = NULL;
struct Request NullRequest = {0};

pthread_mutex_t mutex;

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

   //pthread_mutex_init(&mutex,NULL);
   int nSeats=atoi(argv[1]);
   struct Seat allSeats[nSeats];
   for(int i = 0; i < nSeats; i++){
	allSeats[i].pid = -1;                   // lugar livre se pid == -1
   }



   createRequestFIFO();
   openRequestFIFO();

   createMainThread();

   //printRequest(request);
   
   int n=atoi(argv[2]);
  // for(int i = 0; i < n; i++){
	createTicketOfficeThread(allSeats);
   //}

    pthread_join(main_thread_tid,NULL);
    unlink("requests");
}

void createRequestFIFO(){
   mkfifo("requests", 0660);
}

void openRequestFIFO(){
   
   if((REQUEST_FD = open("requests",O_RDONLY)) < 0){
	printf("Error when opening the request FIFO\n");
	exit(1);
   }

}

struct Request tryToReadRequest(){
  struct Request r;
  while (1)
  {
      if(read(REQUEST_FD,&r,sizeof(struct Request)) > 0){
          return r;
      }

  }
    return r;
}


void createMainThread(){

   pthread_create(&main_thread_tid, NULL, main_thr_func, NULL);
}


/*
Depois as threads auxiliares (bilheteiras) vão ao buffer buscar estes requests do buffer.
*/
void createTicketOfficeThread(struct Seat *allSeats){

   pthread_t tid;
   pthread_create(&tid, NULL, ticket_office_thr_func, allSeats);
   //pthread_join(tid, NULL);
}

/*
Falta por esta funcao a fazer alguma coisa
*/
void *ticket_office_thr_func(void *arg) {

   printf("New Ticket Office Thread\n");

   pthread_mutex_lock(&mutex);

   struct Seat *seats = arg;
   struct Request request;

   while(1) {
       if (buffer != NULL) {
            request = (*buffer);
            break;
        }
       else printf("BUFFER NULL\n");
   }

   buffer = NULL;

   int stat;
   int fdans;

   printf("ANTES %ld\n", (long)request.pid);
    fdans=openAnsFIFO(request.pid);
    printf("DEPOIS%d\n",fdans);

    if((stat=checkRequestConditions(seats,request)) != 0){
        printf("inside if\n");
        write(fdans,&stat,sizeof(stat));
    }
    printf("SAIU\n");

    int answer[MAX_CLI_SEATS+1];
    write(fdans,answer,sizeof(answer));

    pthread_mutex_unlock(&mutex);

   return NULL;
}

/*
Falta por esta funcao a fazer alguma coisa
Provavelmente vai ter de receber alguma cena (os requests) todos
*/
void *main_thr_func(void *arg){

    struct Request request;

    buffer = malloc(sizeof(buffer));

    printf("Main Thread Called\n");

   while(1){                            //LER REQUEST
           request = tryToReadRequest();
           printRequest(request);
           (*buffer) = request;
   }

    return NULL;
}

void printRequest(struct Request r) {
    printf("REQUEST:\n");
    printf("%d\n", r.num_wanted_seats);
    printf("%ld\n", (long) r.pid);
    printf("%s\n",r.pref_seat_list);
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

    printf("peidoinicial\n");
    int lugarocupado = 0;
    const char s[2] = " ";
    int lugar = -1;
    char * lugares;
    int counter = 0;

    printf("peido1\n");
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
    printf("peido3\n");

    return 0;
}

int openAnsFIFO(pid_t pid){
    int fd;
    char str[50];
    sprintf(str,"%s%ld", "ans", (long)pid);
    fd=open(str,O_WRONLY);
    return fd;
}
