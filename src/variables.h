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


//numero maximo de uma sala
#define MAX_ROOM_SEATS 9999
//numero maximo de lugares pretendidos
#define MAX_CLI_SEATS 99
//tem haver com guardar nos ficheiros também
#define WIDTH_PID 5
//tem haver com guardar nos ficheiros também
#define WIDTH_XXNN 5
//isto tem haver com guardar nos ficheiros
#define WIDTH_SEAT 4





struct Seat {
    pid_t pid;		//pid = pid do cliente (se ocupado) OU -1 (se não ocupado)
};

struct Request {
    pid_t pid;
    int time_out;
    int num_wanted_seats;
    char * pref_seat_list;
};
