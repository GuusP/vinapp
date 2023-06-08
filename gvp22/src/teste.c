#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "vina.h"

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    FILE *test;
    test = fopen("test_file", "r");
    Membro membro;
    membro.location = 21;
    membro.size = 12;
    fwrite(&membro, sizeof(Membro), 1, test);
    fread(&membro, sizeof(Membro), 1, test);
    printf("size: %ld - location: %u\n", membro.size, membro.location);
    printf("%ld", ftell(test));
    return 0;
}