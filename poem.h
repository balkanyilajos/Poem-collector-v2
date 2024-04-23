#ifndef POEM_H
#define POEM_H

#include <stdlib.h>
#include <stdio.h>

struct Poem {
    char* title;
    char* content;
};

typedef struct Poem Poem;

void freePoem(Poem* poem) {
    free(poem->title);
    free(poem->content);
    free(poem);
}

void printPoem(Poem* poem) {
    printf("title - %s\n", poem->title);
    printf("content:\n%s\n", poem->content);
}

#endif