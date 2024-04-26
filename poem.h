#ifndef POEM_H
#define POEM_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define POEM_PRIORITY 1

struct Poem {
    char* title;
    char* content;
};

typedef struct Poem Poem;

struct PoemMessage {
    long mtype;
    int poemID;
};

typedef struct PoemMessage PoemMessage;

void freePoem(Poem* poem) {
    free(poem->title);
    free(poem->content);
    free(poem);
}

void printTitle(Poem* poem) {
    printf("title - %s\n", poem->title);
}

void printContent(Poem* poem) {
    printf("content:\n%s\n", poem->content);
}

void printPoem(Poem* poem) {
    printTitle(poem);
    printContent(poem);
}

void sendPoem(int messageID, int poemID) {
    const PoemMessage message = { POEM_PRIORITY, poemID };
    msgsnd(messageID, &message, sizeof(int), 0);
}

int receivePoem(int messageID) {
    PoemMessage message;
    msgrcv(messageID, &message, sizeof(Poem), POEM_PRIORITY, 0);
    return message.poemID;
}

#endif