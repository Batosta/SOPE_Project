#include "usefull.h"

//Function that writes the message to the filename.txt
void writeMessage(char *filename, char *message){

   FILE *file;
   file = fopen(filename, "a+");
   fprintf(file, message);
}

//Function that creates (if not yet) 4 .txt files or just clears them if already created
void cleanMessages(){
   
   fopen("sbook.txt", "w");
   fopen("cbook.txt", "w");
   fopen("slog.txt", "w");
   fopen("clog.txt", "w");
}
