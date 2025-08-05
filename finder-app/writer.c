#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>

int main(int argc, char** argv){

  openlog("mylog", 0, LOG_USER);
  if(argc != 3){
    syslog(LOG_ERR, "wrong number of arguments %d =! 2\n", argc-1);
    exit(1);
  }
  
  FILE* fptr=NULL;
  fptr = fopen(argv[1], "w");
  if(!fptr){
    syslog(LOG_ERR, "failed opening file");
    exit(1);
  }

  fprintf(fptr, "%s\n", argv[2]);
  syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
  fclose(fptr);
}
