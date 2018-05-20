#include "usefull.h"

//Functions for the Request FIFO
void createRequestFIFO();
void openRequestFIFO();
void readRequest();
void closeRequestFIFO();

//Functions for the Answer FIFO
void openAnswerFIFO(const char* fifoName);
void sendAnswer(const char* fifoName, const char* message);

//Functions for the threads
void createTicketOfficeThread(int id);
void * ticketOfficeThread(void *arg);
