#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define MAX_ROOM_SEATS 9999             /* maximum number of room seats/tickets available       	*/
#define MAX_CLI_SEATS 99                /* maximum number of seats/tickets per request          	*/
#define WIDTH_PID 5                     /* length of the PID string                             	*/
#define WIDTH_XXNN 5                    /* length of the XX.NN string (reservation X out of N) 		*/
#define WIDTH_SEAT 4                    /* length of the seat number id string                  	*/

#define DELAY()  usleep(1000*1000); 	/* used to simulate the delay on executing some operations	*/


//Struct which symbolizes the request done by a client
struct Request {
	int pid;				//Client's PID
	int num_wanted_seats;			//Client's number of wanted seats
	int num_pref_seats;			//Client's number of preferred seats
	int pref_seat_list[MAX_ROOM_SEATS];	//Client's array of preferred seats
};

//Struct wich symbolizes the seats of the room
struct Seat {
	int pid;						//pid of the client , 0 if it is free
	pthread_mutex_t seat_mut;	//mutex to book the seat
};

//Struct which symbolizes the answer given to a client
struct Answer {
	struct Request req;			//Request that created this answer
	int error;				//Number of the error, 0 in success	
	int num_reserved;			//Number of seats reserved, to know the number on the res_list
	int res_list[MAX_CLI_SEATS];		//Array with the seats reserved
	int id;					//Ticket Office ID
};


//Functions that will take care of the writing on the .txt files
void writeMessage(FILE *file, char *message);
void cleanMessages();
char * errorToChar(int error);
