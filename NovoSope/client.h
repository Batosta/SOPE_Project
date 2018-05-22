#include "usefull.h"

//Functions for the Request FIFO
void openRequestFIFO();
void sendRequest(struct Request r);


//Functions for the Answer FIFO
void createAnswerFIFO();
void openAnswerFIFO();
void closeAnswerFIFO();
void readAnswer();
void handleAnswer();

//Functions to write in files
void writeLog();
void writeBook();
