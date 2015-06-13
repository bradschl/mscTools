#include <stdio.h>

struct Params_S
{
    const char* outputFile;
    const char* shadowFile;
};


int main(int argc, char** argv)
{

    if(argc != 3)
    {
        fprintf(stderr, "Invalid parameters.\n");
        return -1;
    }

    struct Params_S params;
    params.outputFile = argv[1];
    params.shadowFile = argv[2];

    


    return 0;
}

