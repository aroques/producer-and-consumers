#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "helpers.h"

char** split_string(char* str, char* delimeter) {
    char* substr;
    char** strings = malloc(10 * sizeof(char));

    substr = strtok(str, delimeter);

    int i = 0;
    while (substr != NULL)
    {
        strings[i] = malloc(20 * sizeof(char));
        strings[i] = substr;
        substr = strtok(NULL, delimeter);
        i++;
    }

    return strings;

}

void add_signal_handler() {
  struct sigaction act;
  act.sa_handler = sigint_handler;
  act.sa_flags = 0;
  if ( ( sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGTERM, &act, NULL)  == -1) ) {
      perror("Failed to set up interrupt");
      exit(1);
  }
}

void sigint_handler(int s) {
    // kill children processes and abort
  fprintf(stderr, "\nsig num received: %d\n", s);
  fprintf(stderr, "exiting...\n");

  FILE * fp;
  int i;
  /* open the file for writing*/
  char filename[50];
  sprintf(filename, "/home/alec/umsl/4760/2/src/test%d.txt", getpid());
  fp = fopen (filename, "w");

  /* write 10 lines of text into the file stream*/
  for(i = 0; i < 10; i++){
      fprintf(fp, "This is line %d\n",i + 1);
  }

  /* close the file*/
  fclose (fp);

  perror("test");
  exit(1);
}

