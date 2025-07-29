#include <stdio.h>
#define TURBO_BASE64_IMPLEMENTATION
#include "turbo_neon_base64.h"




int main(int argc, char *argv[])
{
    char input[] = "hello world!";
    char output[1024] = {0};

    size_t output_size = tb64enc(input, strlen(input), output);


    printf("hello world!  %s\n", output);

    return 0;
}