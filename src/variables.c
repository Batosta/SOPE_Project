#include "variables.h"

void writeMessage(char *filename, char *message){

   FILE *file;
   file = fopen(filename, "a+");
   fprintf(file, message);
}

void cleanMessages(){
   
   fopen("sbook.txt", "w");
   fopen("cbook.txt", "w");
   fopen("slog.txt", "w");
   fopen("clog.txt", "w");

}
