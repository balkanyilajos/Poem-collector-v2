#ifndef POEM_COLLECTOR_H
#define POEM_COLLECTOR_H

#include "poem.h"
#include "poem_container.c"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <signal.h>

void run(const char *filename, const char* rootPath);
Poem* readPoemFromConsole(void);
int readNumberFromConsole(void);
char* readStringFromConsole(char end);
void trim(char* str);
void setTimer(void);
void arriveHandler(int signum);
bool any(int* arr, int size, int value);

void MemoryAllocationException(void);
void IOException(void);
void ForkException(void);
void PipeException(void);

#endif