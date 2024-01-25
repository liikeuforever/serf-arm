#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_BLOCK_SIZE 100
#define DIR "../../main/resources/floating/"
typedef struct
{
    FILE *file;
    int blockSize;
    int end;
} BlockReader;

BlockReader *createBlockReader(const char *fileName, int blockSize)
{
    BlockReader *reader = (BlockReader *)malloc(sizeof(BlockReader));
    if (reader == NULL)
    {
        fprintf(stderr, "Error allocating memory for BlockReader.\n");
        exit(EXIT_FAILURE);
    }
    char *file_end = new char[strlen(DIR) + strlen(fileName) + 1];
    strcpy(file_end, DIR);
    strcat(file_end, fileName);

    reader->file = fopen(file_end, "r");
    if (reader->file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        free(reader);
        exit(EXIT_FAILURE);
    }

    reader->blockSize = blockSize;
    reader->end = 0;

    return reader;
}

void closeBlockReader(BlockReader *reader)
{
    if (reader != NULL)
    {
        fclose(reader->file);
        free(reader);
    }
}

double *nextSingleBlock(BlockReader *reader)
{
    if (reader->end)
    {
        return NULL;
    }

    double *floatings = (double *)malloc(sizeof(double) * reader->blockSize);
    if (floatings == NULL)
    {
        fprintf(stderr, "Error allocating memory for floatings.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char line[MAX_LINE_LENGTH];
    while (i < reader->blockSize && fgets(line, sizeof(line), reader->file) != NULL)
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }

        floatings[i++] = strtof(line, NULL);
    }

    if (i < reader->blockSize)
    {
        reader->end = 1;
    }

    if (i == 0)
    {
        free(floatings);
        return NULL;
    }

    return floatings;
}

int main()
{
    const char *fileName = "Air-pressure.csv";
    int blockSize = 1000;

    BlockReader *reader = createBlockReader(fileName, blockSize);

    double *block;
    while ((block = nextSingleBlock(reader)) != NULL)
    {
        for (int i = 0; i < blockSize; ++i)
        {
            printf("%d  %f\n", i, block[i]);
        }
        free(block);
    }

    closeBlockReader(reader);

    return 0;
}