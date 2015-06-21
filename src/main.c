/**
 * Copyright (c) 2015, Bradley Kim Schleusner < bradschl@gmail.com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <popt.h>

#include <mscTools/IoBlock.h>
#include <mscTools/RAQ.h>
#include <mscTools/Version.h>


typedef enum
{
    PARAM_NONE = 0,
    PARAM_VERSION = 1
} CommandParams_E;


// RBC Mode options
static      int         rbcByteLimit = 0;
// TODO: Make these arguments
static      int         rbcMaxBlockSize     = 4096;
static      int         rbcMinBLockSize     = 64;

// Erase Mode options
static      int         eraseBlockSize      = 4096;

// Command tables
static struct poptOption rbcOptions[] =
{
    { "limit", 'L', POPT_ARG_INT, &rbcByteLimit, PARAM_NONE,
    "- copy byte limit, default is the input file size.", " byte limit" },

    POPT_TABLEEND
};

static struct poptOption sizeOptions[] =
{
    POPT_TABLEEND
};


static struct poptOption eraseOptions[] =
{
    { "erase-bs", 0, POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &eraseBlockSize, PARAM_NONE,
    "- erase block size", "bytes"},

    POPT_TABLEEND
};


static struct poptOption optionTable[] =
{
    {NULL, (char)-1, POPT_ARG_INCLUDE_TABLE, rbcOptions, 0,
    "Random Block Copy Mode: rbc <input file> <output file> [OPTIONS...]", NULL},

    {NULL, (char)-1, POPT_ARG_INCLUDE_TABLE, sizeOptions, 0,
    "Get Input Size Mode: size <input file>", NULL},

    {NULL, (char)-1, POPT_ARG_INCLUDE_TABLE, eraseOptions, 0,
    "Erase Mode: erase <input file>", NULL},

    // TODO: Add max block size
    // TODO: Add min block size

    {"version", 'v', POPT_ARG_NONE, NULL, PARAM_VERSION, "Version", NULL},

    POPT_AUTOHELP
    POPT_TABLEEND
};



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

        if(rbcByteLimit <= 0)
        {
            rbcByteLimit = IoBlock_size(&input);
        }

        RAQHandle raqHandle = RAQ_init(rbcByteLimit, rbcMinBLockSize, rbcMaxBlockSize);

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
    printf("mscTools version: %s\n", getVersionString());
    return 0;
}

static int eraseTarget(const char* outputFIle)
{
    IoBlockHandle output;
    if(IO_SUCCESS != IoBlock_open(&output, outputFIle, true, false))
    {
        fprintf(stderr, "Could not open '%s' for writing.\n", outputFIle);
        return -1;
    }

    uint8_t eraseBlock[eraseBlockSize];
    memset(eraseBlock, 0xFF, sizeof(eraseBlock));

    size_t bytesToErase = IoBlock_size(&output);
    size_t address = 0;
    while(address < bytesToErase)
    {
        size_t writeSize = bytesToErase - address;
        if(writeSize > sizeof(eraseBlock))
        {
            writeSize = sizeof(eraseBlock);
        }

        printf("Writing %ld bytes to %ld...\n", writeSize, address);
        // TODO: Check return codes
        IoBlock_write(&output, address, eraseBlock, writeSize);

        address += writeSize;
    }

    IoBlock_close(&output);
    return 0;
}

int main(int argc, const char** argv)
{
    int ret = 0;


    poptContext con = poptGetContext(NULL, argc, argv, optionTable, 0);
    poptSetOtherOptionHelp(con, "<COMMAND> [OPTIONS...]");

    char c;
    while((c = poptGetNextOpt(con)) >= 0)
    {
        char * arg = poptGetOptArg(con);

        switch(c)
        {
            case PARAM_VERSION:
            {
                // TODO: Don't return here
                return versionInfo();
            }

            case PARAM_NONE:
            default:
                break;
        }

        if(c < 0)
        {
            fprintf(stderr, "Parse error: %s %s\n",
                poptBadOption(con, POPT_BADOPTION_NOALIAS),
                poptStrerror(c));

            // TODO: Don't return here
            return -1;
        }

        if(arg)
        {
            free(arg);
        }
    }

    bool parseFail = false;

    const char ** av = poptGetArgs(con);
    if(NULL == av)
    {
        parseFail = true;
    }
    else if(0 == strcmp("rbc", *av))
    {
        ++av;
        const char* inputFile   = *av;  ++av;
        const char* outputFile  = *av;  ++av;

        if(    (NULL == inputFile)
            || (NULL == outputFile)
            || (rbcByteLimit < 0))
        {
            parseFail = true;
        }
        else
        {
            ret = randBlockCopy(inputFile, outputFile);
        }
    }
    else if(0 == strcmp("size", *av))
    {
        ++av;
        const char* inputFile   = *av;  ++av;
        if(NULL == inputFile)
        {
            parseFail = true;
        }
        else
        {
            ret = getSizeInfo(inputFile);
        }
    }
    else if(0 == strcmp("erase", *av))
    {
        ++av;
        const char* outputFile  = *av;  ++av;
        if(    (NULL == outputFile)
            || (eraseBlockSize < 1))
        {
            parseFail = true;
        }
        else
        {
            ret = eraseTarget(outputFile);
        }
    }
    else
    {
        parseFail = true;
    }

    if(parseFail)
    {
        poptPrintUsage(con, stderr, 0);
        ret = -1;
    }

    con = poptFreeContext(con);
    return ret;
}

