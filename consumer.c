#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "global_constants.h"

void add_signal_handler();
void handler(int s);

int main (int argc, char *argv[]) {

//    int segment_id = atoi(argv[1]);
//
//    /* Attach the shared memory segment. */
//    char* shared_memory = attach_shared_memory(segment_id);
    //int i = atoi(argv[1]);

    add_signal_handler();

	printf("hello from consumer!\n");
    printf("argv[0] = %s\n", argv[0]);
    printf("argv[1] = %s\n", argv[1]);
    printf("argv[2] = %s\n", argv[2]);
    printf("argv[3] = %s\n", argv[3]);
	//printf("my number is = %s!\n", argv[1]);

    while(1);
    return 0;
}


void add_signal_handler() {
  struct sigaction act;
  act.sa_handler = handler;
  act.sa_flags = 0;
  if ( ( sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL)  == -1) ) {
      perror("Failed to set up interrupt");
      exit(1);
  }
}


void handler(int s) {
    // kill children processes and abort
  fprintf(stderr, "\nsig num received in consumer: %d\n", s);
  fprintf(stderr, "exiting...\n");

  FILE * fp;
  int i;
  /* open the file for writing*/
  char filename[50];
  sprintf(filename, "/home/alec/umsl/4760/2/src/test%d.txt", getpid());
  fp = fopen (filename,"w");

  /* write 10 lines of text into the file stream*/
  for(i = 0; i < 10;i++){
      fprintf(fp, "This is line %d\n",i + 1);
  }

  /* close the file*/
  fclose (fp);


  perror("test");
  exit(1);
}
