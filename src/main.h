#ifndef _REVOLVER_src_main_h_
#define _REVOLVER_src_main_h_ 1


#define _GNU_SOURCE


#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/revolver.h"


#define perror(x) if (DEBUG) perror(x)


void print_help(const char *prog_name);


#endif
