#include "usefull.h"

//Functions for the Request FIFO
void createRequestFIFO();
void openRequestFIFO();
void mainLoop();
void readRequest();
void closeRequestFIFO();

//Functions for the Answer FIFO
void openAnswerFIFO(int pid, struct Answer * ans);
void sendAnswer(int ANSWER_FD, struct Answer * ans);

//Functions for the threads
void createTicketOfficeThread(int ticketOfficesNumber);
void * ticketOfficeThread(void *arg);
void readBuffer(struct Request * req);
void processRequest(struct Request * req, int id);
int testSomeCond(struct Request * req);
void freeSeat(struct Seat *seats, int seatNum);
void bookSeat(struct Seat *seats, int seatNum, int clientId);
int isSeatFree(struct Seat *seats, int seatNum);
void handleAnswer(struct Answer * ans);

//Functions for writing on the files
void openLog();
void writeLog(struct Answer * ans);
void writeBook();
