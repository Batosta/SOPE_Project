#include "usefull.h"

//Functions for the Request FIFO
void openRequestFIFO();
void sendRequest(int nrlug,char pref[]);


//Functions for the Answer FIFO
void createOpenAnswerFIFO();
void readAnswer();
