#include "poem_collector.h"

static const char *MENU_TEXT = "--------------------------\n"
                               "| [1] Add a new poem     |\n"
                               "| [2] Print a poem       |\n"
                               "| [3] Print poems' title |\n"
                               "| [4] Delete a poem      |\n"
                               "| [5] Modify a poem      |\n"
                               "| [6] Watering           |\n"
                               "| [7] Exit               |\n"
                               "--------------------------\n"
                               "Write here your command: ";

void run(const char *filename, const char* rootPath)
{
    setTimer();
    bool running = true;
    const char *const separator = "-----";
    const char *const separatorWithNewLines = "\n-----\n";

    Poem *poem;
    int id, inputValue, status, childNum = 4, childsInitValue = 0;
    int fd[2];
    int poems[2];
    int selectedPoem;
    int randomPoems[2];
    int poemNumber;
    pid_t childs[childNum];
    pid_t pid;
    pid_t parent = getpid();
    key_t key = ftok(rootPath, 1);
    int messageID = msgget(key, 0600 | IPC_CREAT);

    if (pipe(fd) == -1)
    {
        PipeException();
    }

    while (running)
    {
        printf("%s", MENU_TEXT);
        inputValue = readNumberFromConsole();
        int selectedChildIndex = rand() % childNum;
        memset(childs, childsInitValue, sizeof(childs));
        switch (inputValue)
        {
        case 1:
            printf("I am waiting for your poem (to stop it, press @ or EOF):\n");
            poem = readPoemFromConsole();
            if (poem == NULL)
                MemoryAllocationException();
            if (safeToFile(filename, poem, separatorWithNewLines))
            {
                freePoem(poem);
                IOException();
            }
            freePoem(poem);
            break;
        case 2:
            printf("Enter the id of the selected poem: ");
            id = readNumberFromConsole();
            poem = readFromFile(filename, id, separator);
            if (poem == NULL)
                IOException();
            if (strlen(poem->title) == 0)
            {
                printf("The ID doesn't exists!\n");
            }
            else
            {
                printPoem(poem);
            }

            freePoem(poem);
            break;
        case 3:
            id = 1;
            poem = readFromFile(filename, id, separator);
            if (poem == NULL)
                IOException();

            printf("titles:\n");
            while (strlen(poem->title) != 0)
            {
                printf("%d: %s\n", id, poem->title);
                freePoem(poem);

                id += 1;
                poem = readFromFile(filename, id, separator);
                if (poem == NULL)
                    IOException();
            }
            freePoem(poem);
            break;
        case 4:
            printf("Enter the id of the selected poem: ");
            id = readNumberFromConsole();
            freePoem(deleteFromFile(filename, id, separator));
            break;
        case 5:
            printf("Enter the id of the selected poem: ");
            id = readNumberFromConsole();
            poem = deleteFromFile(filename, id, separator);
            if (strlen(poem->title) == 0)
            {
                printf("The ID doesn't exists!\n");
            }
            else
            {
                printPoem(poem);

                printf("Write here your new Poem: ");
                poem = readPoemFromConsole();
                if (poem == NULL)
                    MemoryAllocationException();

                if (safeToFile(filename, poem, separatorWithNewLines))
                {
                    freePoem(poem);
                    IOException();
                }
            }

            freePoem(poem);
            break;
        case 6:
            signal(SIGUSR1, arriveHandler);
            for (int i = 0; i < childNum; i++)
            {
                childs[i] = fork();
                if (childs[i] < 0)
                    ForkException();

                if (childs[i] == 0)
                {
                    childs[i] = getpid();
                    printf("%d. child process created\n", i + 1);
                    break;
                }
            }

            if (getpid() == childs[selectedChildIndex]) // selected child
            {
                close(fd[1]);
                usleep(500000);
                printf("The fittest boy left home. (%d)\n", childs[selectedChildIndex]);
                sleep(1);
                printf("The fittest boy arrived at Barátfa. \n");
                printf("Sent signal...\n");
                kill(getppid(), SIGUSR1);
                usleep(500000);
                read(fd[0], poems, sizeof(int));
                read(fd[0], poems+1, sizeof(int));
                close(fd[0]);
                printf("1. Poem:\n");
                poem = readFromFile(filename, poems[0], separator);
                printTitle(poem);
                freePoem(poem);

                printf("\n");

                printf("2. Poem:\n");
                poem = readFromFile(filename, poems[1], separator);
                printTitle(poem);
                freePoem(poem);

                printf("\n");

                selectedPoem = poems[(int)rand() % 2];
                printf("Selected poem:\n");
                poem = readFromFile(filename, selectedPoem, separator);

                sendPoem(messageID, selectedPoem);
                printf("Message sent to parent...\n");
                usleep(500000);
                kill(getppid(), SIGUSR1);
                usleep(1000000);
                printf("The selected poem was recited by the child.\n");
                printTitle(poem);
                freePoem(poem);
            }

            if(getpid() == parent) { // parent
                pause();
                printf("Arrived signal...\n");
                close(fd[0]);
                poemNumber = getNumberOfPoems(filename, separator);
                
                printf("Number of poems: %d\n", poemNumber);
                randomPoems[0] = (int)rand() % (poemNumber) + 1;
                write(fd[1], randomPoems, sizeof(int));
                randomPoems[1] = (int)rand() % (poemNumber) + 1;
                write(fd[1], randomPoems + 1, sizeof(int));
                close(fd[1]);
                
                pause();
                selectedPoem = receivePoem(messageID);
                usleep(500000);
                printf("The sent poem arrived from child (ID: %d).\n", selectedPoem);
                
            }
            else { // childs
                exit(0);
            }

            for(int i = 0; i < childNum; i++) {
                pid = wait(&status);
                printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
            }

            break;
        case 7:
            running = false;
            break;
        }

        printf("\n");
    }
}

Poem *readPoemFromConsole()
{
    Poem *poem = (Poem *)malloc(sizeof(Poem));
    if (poem == NULL)
        return NULL;

    printf("title - ");
    poem->title = readStringFromConsole('\n');
    if (poem->title == NULL)
    {
        freePoem(poem);
        return NULL;
    }

    printf("content:\n");
    poem->content = readStringFromConsole('@');
    if (poem->content == NULL)
    {
        freePoem(poem);
        return NULL;
    }

    trim(poem->title);
    trim(poem->content);

    return poem;
}

char *readStringFromConsole(char end)
{
    int buffer = 2;

    char *storage = (char *)malloc(buffer * sizeof(char));
    if (storage == NULL)
    {
        return NULL;
    }

    int i = 0;
    for (char c = getchar(); c != EOF && c != end; i += 1, c = getchar())
    {
        if (i >= buffer - 2)
        {
            buffer *= 2;
            storage = (char *)realloc(storage, buffer * sizeof(char));
            if (storage == NULL)
            {
                free(storage);
                return NULL;
            }
        }
        storage[i] = c;
    }
    storage[i] = '\0';

    return storage;
}

int readNumberFromConsole()
{
    char line[16];
    fgets(line, sizeof(line), stdin);

    return atoi(line);
}

void trim(char *str)
{
    int start = 0;
    while (isspace(str[start]))
    {
        start += 1;
    }

    if (str[start] == '\0')
    {
        str[0] = '\0';
        return;
    }

    int end = strlen(str) - 1;
    while (isspace(str[end]))
    {
        end -= 1;
    }
    str[end + 1] = '\0';

    if (start > 0)
    {
        memmove(str, str + start, end - start + 2);
    }
}

void setTimer()
{
    srand(time(NULL));
}

void arriveHandler(int signum) {}

bool any(int *arr, int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == value)
        {
            return true;
        }
    }
    return false;
}

// error functions

void MemoryAllocationException()
{
    printf("Memory allocation error!");
    abort();
}

void IOException()
{
    printf("Input/Output error!");
    abort();
}

void ForkException()
{
    printf("Fork error");
    abort();
}

void PipeException() {
    printf("Fork error");
    abort();
}