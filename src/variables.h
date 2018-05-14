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

#define DELAY()  usleep(1000*1000);      //  NAO É USADA AINDA - Espera 1seg

struct Seat {
    pid_t pid;		//pid = pid do cliente (se ocupado) OU -1 (se não ocupado)
};

struct Request {
    pid_t pid;
    int time_out;
    int num_wanted_seats;
    char * pref_seat_list;
};


void writeMessage(char *filename, char *message);
void cleanMessages();
