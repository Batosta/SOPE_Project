#include "usefull.h"

//Function that writes the message to the filename.txt
void writeMessage(FILE *file, char *message){

   fprintf(file, "%s\n",message);
}

//Function that creates (if not yet) 4 .txt files or just clears them if already created
void cleanMessages(){
   
   fopen("sbook.txt", "w");
   fopen("cbook.txt", "w");
   fopen("slog.txt", "w");
   fopen("clog.txt", "w");
}

char * errorToChar(int error){
	char ret[3];
	switch(error){
		case -1:sprintf(ret,"MAX");
			break;
		case -2:sprintf(ret,"NST");
			break;
		case -3:sprintf(ret,"IID");
			break;
		case -4:sprintf(ret,"ERR");
			break;
		case -5:sprintf(ret,"NAV");
			break;
		case -6:sprintf(ret,"FUL");
			break;
		case -7:sprintf(ret,"OUT");
			break;
		default:sprintf(ret,"NOP");
			break;
	}
	char * ret1=ret;
	return ret1;
}
