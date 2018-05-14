#include "variables.h"

#define DELAY()  usleep(1000*1000);                                       //  NAO É USADA AINDA - Espera 1seg

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
    char * lugar;
    char * lugares;
    int counter = 0;

    printf("peido1\n");
    if(request.num_wanted_seats >= MAX_CLI_SEATS) {
        printf("num_wanted_seats must be a value below MAX_CLI_SEATS.\n");
        return -1;
    }

    printf("%s",request.pref_seat_list);

    printf("peido2.1\n");
    lugar = strtok(request.pref_seat_list, s);
    printf("peido2\n");
    while(lugar != NULL) {

        if(atoi(lugar) > MAX_ROOM_SEATS || atoi(lugar) <= 0) {
            printf("All wanted seats must be values above 0 and below MAX_ROOM_SEATS.\n");
            return -3;
        }

        if(isSeatFree(seats,atoi(lugar))) {
            bookSeat(seats, atoi(lugar), request.pid);
        } else {
            lugarocupado = 1;
            break;
        }

        lugar = strtok(NULL, s);
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

    lugares = request.pref_seat_list;

    lugar = strtok(lugares,s);

    if(lugarocupado == 1) {
        lugar = strtok(lugares, s);
        while (lugar != NULL) {
            freeSeat(seats,atoi(lugar));
            lugar = strtok(lugares, s);
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