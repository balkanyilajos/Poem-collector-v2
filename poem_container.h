#ifndef POEM_CONTAINER_H
#define POEM_CONTAINER_H

#include "poem.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isFileExists(const char* filename);
int createFile(const char* filename);
int safeToFile(const char* filename, const Poem* poem, const char* separator);
Poem* readFromFile(const char* filename, const int poemId, const char* separator);
Poem* deleteFromFile(const char* filename, const int poemId, const char* separator);

#endif
