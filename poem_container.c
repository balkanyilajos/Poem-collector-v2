#include "poem_container.h"

bool isFileExists(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    bool isExists = false;
    if (fp != NULL)
    {
        isExists = true;
        fclose(fp);
    }
    return isExists;
}

int createFile(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        return -2;
    };
    fclose(fp);
    return 0;
}

int safeToFile(const char *filename, const Poem *poem, const char *separator)
{
    if (!strlen(poem->content) || !strlen(poem->title))
    {
        return 0;
    }
    if (!isFileExists(filename))
    {
        if (createFile(filename))
        {
            return -1;
        }
    }

    FILE *fp = fopen(filename, "a");
    fprintf(fp, "%s\n", poem->title);
    fprintf(fp, "%s", poem->content);
    fprintf(fp, "%s", separator);

    fclose(fp);
    return 0;
}

Poem *readFromFile(const char *filename, const int poemId, const char *separator)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return NULL;

    size_t contentBufferSize = 64;
    const size_t maxLineLength = 1028;

    Poem *poem = (Poem *)malloc(sizeof(Poem));
    if (poem == NULL)
        return NULL;

    poem->title = (char *)malloc(1 * sizeof(char));
    if (poem->title == NULL)
    {
        freePoem(poem);
        return NULL;
    }
    poem->title[0] = '\0';

    poem->content = (char *)malloc(contentBufferSize * sizeof(char));
    if (poem->content == NULL)
    {
        freePoem(poem);
        return NULL;
    }
    poem->content[0] = '\0';

    char line[maxLineLength];

    int currentId = 1;
    const size_t separatorLength = strlen(separator);
    while (poemId != currentId && fgets(line, maxLineLength, fp) != NULL)
    {
        if (strlen(line) >= separatorLength && strstr(line, separator) != NULL)
        {
            currentId += 1;
        }
    }

    if (fgets(line, maxLineLength, fp) != NULL)
    {
        int lineLength = strlen(line) + 1;
        free(poem->title);
        poem->title = (char *)malloc((lineLength + 1) * sizeof(char));
        if (poem->title == NULL)
        {
            freePoem(poem);
            return NULL;
        }

        poem->title[0] = '\0';
        strcat(poem->title, line);
        poem->title[lineLength - 1] = '\0';
    }

    size_t contentLength = 1;
    while (fgets(line, maxLineLength, fp) != NULL && strcmp(line, separator))
    {
        size_t lineLength = strlen(line) + 1;
        contentLength += lineLength;
        if (strlen(line) >= separatorLength && strstr(line, separator) != NULL)
        {
            break;
        }
        if (contentLength >= contentBufferSize)
        {
            contentBufferSize *= 2;
            poem->content = (char *)realloc(poem->content, contentBufferSize * sizeof(char));
            if (poem->content == NULL)
            {
                freePoem(poem);
                return NULL;
            }
        }

        strcat(poem->content, line);
    }

    poem->content[strlen(poem->content) - 1] = '\0';

    fclose(fp);
    return poem;
}

Poem *deleteFromFile(const char *filename, const int poemId, const char *separator)
{
    Poem *poem = readFromFile(filename, poemId, separator);

    const char *tempFilename = "temp.txt";
    FILE *fp = fopen(filename, "r");
    FILE *tempfp = fopen(tempFilename, "w");
    if (fp == NULL || tempfp == NULL)
        return NULL;

    const size_t maxLineLength = 1028;
    char line[maxLineLength];

    int currentId = 1;
    while (fgets(line, maxLineLength, fp) != NULL)
    {
        if (currentId != poemId)
        {
            fputs(line, tempfp);
        }
        if (strstr(line, separator) != NULL)
        {
            currentId += 1;
        }
    }

    fclose(fp);

    remove(filename);
    rename(tempFilename, filename);

    fclose(tempfp);

    return poem;
}

int getNumberOfPoems(const char *filename, const char *separator)
{
    int counter = 0;
    int id = 1;
    Poem* poem = readFromFile(filename, id, separator);
    if (poem == NULL)
        return -1;

    while (strlen(poem->title) != 0)
    {
        freePoem(poem);
        counter += 1;
        id += 1;
        poem = readFromFile(filename, id, separator);
        if (poem == NULL)
            return -1;
    }
    freePoem(poem);

    return counter;
}
