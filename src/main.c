#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <Entropy.h>
#include <IoBlock.h>
#include <RAQ.h>
#include <Version.h>

typedef enum
{
    UNKNOWN,
    RAND_BLOCK_COPY,
    SIZE_INFO,
    VERSION_INFO
} Command;

// TODO: Make these arguments
static const uint16_t MAX_BLOCK_SIZE    = 4096;
static const uint16_t MIN_BLOCK_SIZE    = 64;


static int randBlockCopy(const char* inputFile, const char* outputFile)
{
    printf("Random block copy mode.\n");
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

            printf("Copying %d bytes to address %d...\n", (int)block.length, (int)block.address);

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

static int getSizeInfo(const char* inputFile)
{
    IoBlockHandle input;
    if(IO_SUCCESS != IoBlock_open(&input, inputFile, true, true))
    {
        fprintf(stderr, "Could not open '%s' for reading.\n", inputFile);
        return -1;
    }

    size_t fileSize = IoBlock_size(&input);
    printf("'%s' read as %lu bytes\n", inputFile, fileSize);

    IoBlock_close(&input);
    return 0;
}

static int versionInfo(void)
{
    printf("Git commit: %s\n", getVersionString());
    return 0;
}

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
    if(0 == strcmp("rbc", *argv))
    {
        cmd = RAND_BLOCK_COPY;
    }
    else if(0 == strcmp("size", *argv))
    {
        cmd = SIZE_INFO;
    }
    else if(0 == strcmp("version", *argv))
    {
        cmd = VERSION_INFO;
    }
    argc--;
    argv++;

    if((RAND_BLOCK_COPY == cmd) && (2 == argc))
    {
        return randBlockCopy(argv[0], argv[1]);
    }
    else if((SIZE_INFO == cmd) && (1 == argc))
    {
        return getSizeInfo(argv[0]);
    }
    else if((VERSION_INFO == cmd) && (0 == argc))
    {
        return versionInfo();
    }
    else
    {
        fprintf(stderr, "Invalid parameters.\n");
        return -1;
    }
}

