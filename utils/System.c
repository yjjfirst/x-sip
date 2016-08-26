#include <stdio.h>

unsigned int GetSeed()
{
    unsigned int seed;

    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);

    return seed;
}
