#include "variables.h"

void *ticket_office_thr_func(void *arg);
void openRequestFIFO();
void *main_thr_func(void *arg);
void createTicketOfficeThread();
void createRequestFIFO();
struct Request tryToReadRequest();
void printRequest(struct Request r);
void createMainThread();

int REQUEST_FD;
struct Request buffer = {0};
struct Request NullRequest = {0};

int main(int argc, char *argv[]) {
   
   if (argc!=4){
   	printf("Usage: client <num_room_seats> <num_ticket_offices> <open_time>\n");
   	return -1;
   }
   if(argv[1] < 0){
	printf("num_room_seats must be a value above 0\n");
	return -1;
   }
   if(argv[2] < 0){
	printf("num_ticket_offices must be a value above 0\n");
	return -1;
   }
   if(argv[3] < 0){
	printf("open_time must be a value above 0\n");
	return -1;
   }


   int nSeats=atoi(argv[1]);
   struct Seat allSeats[nSeats];
   for(int i = 0; i < nSeats; i++){
	allSeats[i].pid = -1;
   }

   createRequestFIFO();
   openRequestFIFO();

   createMainThread();

   //printRequest(request);
   
   int n=atoi(argv[2]);
   for(int i = 0; i < n; i++){
	createTicketOfficeThread();
   }

   unlink("requests");
}

void createRequestFIFO(){

   if(mkfifo("requests", 0660) == -1){
	
	printf("Error when creating the request FIFO\n");
	exit(1);
   }
}

void openRequestFIFO(){
   
   if((REQUEST_FD = open("requests",O_RDONLY)) < 0){
	printf("Error when opening the request FIFO\n");
	exit(1);
   }

}

struct Request tryToReadRequest(){
  struct Request r;
  if(read(REQUEST_FD,&r,sizeof(struct Request))==-1){
      printf("Error reading request\n");
  }
    return r;
}


void createMainThread(){
   
   pthread_t tid;
   pthread_create(&tid, NULL, main_thr_func, NULL);
   //pthread_join(tid, NULL);
}


/*
Depois as threads auxiliares (bilheteiras) vão ao buffer buscar estes requests do buffer.
*/
void createTicketOfficeThread(){

   pthread_t tid;

   pthread_create(&tid, NULL, ticket_office_thr_func, NULL);
   //pthread_join(tid, NULL);
}

/*
Falta por esta funcao a fazer alguma coisa
*/
void *ticket_office_thr_func(void *arg) {

   printf("New Ticket Office Thread\n");

   struct Request request = buffer;

   

   return NULL;
}

/*
Falta por esta funcao a fazer alguma coisa
Provavelmente vai ter de receber alguma cena (os requests) todos
*/
void *main_thr_func(void *arg){

    struct Request request={0};

    printf("Main Thread Called");

   while(1){                            //LER REQUEST
       request = tryToReadRequest();
       buffer = request;
   }

    return NULL;
}

void printRequest(struct Request r) {
    printf("%d\n", r.num_wanted_seats);
    printf("%ld\n", (long) r.pid);
    //printf("%s\n",r.pref_seat_list);
}