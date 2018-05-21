#include "usefull.h"

//Functions for the Request FIFO
void createRequestFIFO();
void openRequestFIFO();
void mainLoop();
void readRequest();
void closeRequestFIFO();

//Functions for the Answer FIFO
void openAnswerFIFO(const char* fifoName);
void sendAnswer(const char* fifoName, struct Answer ans);

//Functions for the threads
void createTicketOfficeThread(int id);
void * ticketOfficeThread(void *arg);
void readBuffer(struct Request * req);
void freeSeat(int * seats, int seatNum);
void bookSeat(int * seats, int seatNum, int clientId);
int isSeatFree(int * seats,int seatNum);
