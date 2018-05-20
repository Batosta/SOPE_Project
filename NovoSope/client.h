#include "usefull.h"

//Functions for the Request FIFO
void openRequestFIFO();
void sendRequest(struct Request r);


//Functions for the Answer FIFO
void createOpenAnswerFIFO();
void readAnswer();
