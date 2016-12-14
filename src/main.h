#ifndef _REVOLVER_main_h_
#define _REVOLVER_main_h_ 1


#define _GNU_SOURCE
#define REVOLVER_VERSION "0.0.0"


#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define ERR_REV         -2
#define ERR_REV_FOPEN   -3
#define ERR_REV_FCLOSE  -4
#define ERR_REV_FREAD   -5
#define ERR_REV_FWRITE  -6

#define perror(x) if (DEBUG) perror(x)


void print_help(const char *prog_name);
int open_items_file(FILE **items_file, const char *items_file_path, const char *items_file_mode);
int close_items_file(FILE *items_file, const char *items_file_path);
int push_item(FILE *items_file, const char* item);
int shift_item(char **out, FILE *items_file);
int revolve(const char *items_file_path, const char *command);


#endif
