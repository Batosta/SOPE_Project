#include "variables.h"

/*void writeNumber(char *filename, char *message, int value, int range){

   FILE *file;
   file = fopen(filename, "a+");
   printf("oi1\n");
   char foobar[100] = "";
   sprintf(foobar, "%s%u%s", "." range, "%d");
   sprintf(message, foobar, range, value);
   printf("oi2\n");
   fprintf(file, message);
   printf("oi3\n");
}*/

void writeNumber(char *filename, int number){
   
   FILE *file;
   file = fopen(filename, "a+");

   char str[12];
   sprintf(str, "%d", number);
   fprintf(file, str);
}

void writeMessage(char *filename, char message){

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
