#ifndef HELPERS_H
#define HELPERS_H

char** split_string(char* str, char* delimeter);
char* get_timestamp();
int get_sleep_time();
void print_and_write(char* str, FILE* fp);

#endif
