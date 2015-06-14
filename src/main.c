#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <Entropy.h>
#include <IoBlock.h>
#include <RAQ.h>

typedef enum
{
    UNKNOWN,
    RAND_BLOCK_COPY
} Command;

/*
typedef struct
{
    int     argc;
    char**  argv;

    bool    isFinsihed;
    int     retCode;
} ArgsState;

static inline ArgsState Args_init(int argc, char** argv)
{
    ArgsState state;

    state.argc          = argc;
    state.argv          = argv;
    state.isFinsihed    = false;
    state.retCode       = false;

    return state;
}

static inline bool  Args_isFinsihed(ArgsState* args)
{
    return args->isFinsihed;
}

static inline int   Args_getReturnCode(ArgsState* args)
{
    return args->retCode;
}
*/


static int randBlockCopy(const char* inputFile, const char* outputFile);


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
    if(strcmp("rbc", *argv) == 0)
    {
        cmd = RAND_BLOCK_COPY;
    }
    argc--;
    argv++;

    if((cmd == RAND_BLOCK_COPY) && (argc == 2))
    {
        return randBlockCopy(argv[0], argv[1]);
    }
    else
    {
        fprintf(stderr, "Invalid parameters.\n");
        return -1;
    }
}


static int randBlockCopy(const char* inputFile, const char* outputFile)
{
    // TODO: Make these arguments
    static const uint16_t MAX_BLOCK_SIZE    = 4096;
    static const uint16_t MIN_BLOCK_SIZE    = 64;

    int ret = 0;
    do
    {
        IoBlockHandle input;
        if(IO_SUCCESS != IoBlock_open(&input, inputFile, false, true))
        {
            fprintf(stderr, "Could not open '%s' for reading.\n", inputFile);
            ret = -1;
            break;
        }

        IoBlockHandle output;
        if(IO_SUCCESS != IoBlock_open(&output, outputFile, false, false))
        {
            fprintf(stderr, "Could not open '%s' for writing.\n", outputFile);
            ret = -1;
            break;
        }

        RAQHandle raqHandle = RAQ_init(IoBlock_size(&input), MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);

        while(!RAQ_isEmpty(&raqHandle))
        {
            RAQBlock block = RAQ_popRandom(&raqHandle);

            printf("Reading %d bytes from address %d...\n", (int)block.length, (int)block.address);

            uint8_t buffer[block.length];
            // TODO: Check return codes
            IoBlock_read(&input, block.address, buffer, block.length);
            IoBlock_write(&output, block.address, buffer, block.length);
        }

        RAQ_deinit(&raqHandle);
        IoBlock_close(&output);
        IoBlock_close(&input);
    } while(0);

    return ret;
}

