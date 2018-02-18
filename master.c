#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>

const int PROC_LIMIT = 20;

void print_usage();
int* parse_cmd_line_args(int argc, char *argv[]);

int main (int argc, char *argv[]) {
	int n = 10;							// Default number of consumers
	int* num_consumers = &n;			// Number of consumers to fork
	int proc_count = 0;					// Number of concurrent children
	pid_t childpid = 0;					// Child process ID

    num_consumers = parse_cmd_line_args(argc, argv);

    for (int i = 0; i < 4; i++) {

        if (proc_count == PROC_LIMIT) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpid = fork()) == 0) {
            // Child so...
        	printf("hello from child!\n");
        	printf("proc count: %d, pid: %d\n", proc_count, getpid());
        	break;
        }

        if (childpid == -1) {
            perror("Child failed to fork!\n");
            return EXIT_FAILURE;
        }

        proc_count += 1; // increment because we forked

        if (waitpid(-1, NULL, WNOHANG) > 0) {
            // A child has finished executing
            proc_count -= 1;
        }

    }

    if (childpid > 0) {
        while (wait(NULL) > 0); // wait for all children
    }

    free(num_consumers);

    return EXIT_SUCCESS;

}

int* parse_cmd_line_args(int argc, char *argv[]) {
    int* n = malloc(sizeof(int));

    int option;
    while ((option = getopt (argc, argv, "n:h")) != -1)
    switch (option) {
        case 'h':
            print_usage();
            break;
        case 'n':
            *n = atoi(optarg);
            break;
        default:
            print_usage();
    }

    return n;

}

void print_usage() {
    fprintf(stderr, "Usage: master [-n number of consumers]\n");
    exit(EXIT_SUCCESS);
}
