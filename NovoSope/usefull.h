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


#define MAX_ROOM_SEATS 9999             /* maximum number of room seats/tickets available       */
#define MAX_CLI_SEATS 99                /* maximum number of seats/tickets per request          */
#define WIDTH_PID 5                     /* length of the PID string                             */
#define WIDTH_XXNN 5                    /* length of the XX.NN string (reservation X out of N)  */
#define WIDTH_SEAT 4                    /* length of the seat number id string                  */


//Struct which symbolizes the request done by a client
struct Request {
    int pid;					//Client's PID
    int num_wanted_seats;			//Client's number of wanted seats
    int pref_seat_list[MAX_CLI_SEATS];		//Client's array of preferred seats
};
