#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

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

char* get_timestamp() {
    char* timestamp = malloc(sizeof(char)*10);
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(timestamp, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    return timestamp;
}