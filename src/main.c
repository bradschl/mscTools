#include <stdio.h>

#include "Entropy.h"

typedef enum
{
    UNKNOWN,
    RAND_READ,
    RAND_WRITE
} Command;


static int randomRead(const char* inputFile, const char* outputFile);


int main(int argc, char** argv)
{
    argc--;
    argv++;

    if(argc < 1)
    {
        fprintf(stderr, "Missing command.\n");
        return -1;
    }

    Command cmd = UNKNOWN;
    if(strcmp("rread", *argv) == 0)
    {
        cmd = RAND_READ;
    }
    else if(strcmp("rwrite", *argv) == 0)
    {
        cmd = RAND_WRITE;
    }
    argc--;
    argv++;    

    if((cmd == RAND_READ) && (argc == 2))
    {
        return randomRead(argv[0], argv[1]);
    }
    else if((cmd == RAND_WRITE) && (argc == 2))
    {
        // TODO: Stub
        printf("Not implemented\n");
        return 0;
    }
    else
    {
        fprintf(stderr, "Invalid parameters.\n");
        return -1;
    }
}


static int randomRead(const char* inputFile, const char* outputFile)
{
    EntropyHandle entropy;

    if(!Entropy_init(&entropy))
    {
        fprintf(stderr, "Could not initialize entropy driver.\n");
        return -1;
    }


    unsigned int randData[10];
    Entropy_get(&entropy, randData, sizeof(randData));

    unsigned int i;
    for(i = 0; i < (sizeof(randData)/sizeof(*randData)); ++i)
    {
        printf("0x%08X\n", randData[i]);
    }

    Entropy_deinit(&entropy);
    return 0;
}

