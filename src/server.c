#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

void *ticket_office_thr_func(void *arg);
int openRequestFIFO();
void *main_thr_func(void *arg);
void createTicketOfficeThread();
void createRequestFIFO();

int fd;

struct Seat {

   pid_t pid;		//pid = pid do cliente (se ocupado) OU -1 (se não ocupado)
};

struct Request {

   pid_t pid;
   int num_wanted_seats;
   char * pref_seat_list;
};

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
   Seat allSeats[nSeats];
   for(int i = 0; i < nSeats; i++){
	
	allSeats[i].pid = -1;
   }

   
   createRequestFIFO();
   openRequestFIFO();
   
   int n=atoi(argv[2]);
   for(int i = 0; i < n; i++){

	createTicketOfficeThread();
   }
}

void createRequestFIFO(){

   if(mkfifo("requests", 0660) == -1){
	
	printf("Error when creating the request FIFO\n");
	exit(1);
   }
}

int openRequestFIFO(){
   
   if((fd = open("requests",O_RDONLY)) < 0){
	
	printf("Error when opening the request FIFO\n");
	exit(1);
   }
   
   return fd;
}

void tryToReadRequest(){
  char * buf;
  read(fd,buf,WIDTH_PID);
  request.pid=atoi(buf);
  read(fd,buf,1);
  read(fd,buf,WIDTH_SEAT);
  request.num_wanted_seats=atoi(buf);
  read(fd,buf,1);
/*falta ler os prefered seats, não sei como fazer e nem sei se isto está bem*/
}


void createMainThread(){
   
   pthread_t tid;

   pthread_create(&tid, NULL, main_thr_func, NULL);
   pthread_join(tid, NULL);
}


/*
Depois as threads auxiliares (bilheteiras) vão ao buffer buscar estes requests do buffer.
*/
void createTicketOfficeThread(){

   pthread_t tid;

   pthread_create(&tid, NULL, ticket_office_thr_func, NULL);
   pthread_join(tid, NULL);
}

/*
Falta por esta funcao a fazer alguma coisa
*/
void *ticket_office_thr_func(void *arg) {

   printf("New Ticket Ofiice Thread\n");
   return NULL;
}

/*
Falta por esta funcao a fazer alguma coisa
Provavelmente vai ter de receber alguma cena (os requests) todos
*/
void *main_thr_func(void *arg){

   printf("Main Thread Called");
   while(true){
/*qualquer merda de por num buffer de tamanho unitário, é este buffer que vai ser usado pelas bilheteiras*/
   
   }
   return NULL;
}











