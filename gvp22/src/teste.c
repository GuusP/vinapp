#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include "vina.h"

#define BUFFER_SIZE 1024

int main(){
    char buffer[BUFFER_SIZE];
    FILE *test;
    test = fopen("./test_file", "w");
    if(!test){
        printf("Falha ao abrir arquivo de escrita");
        return 1;
    }
        

    FILE *arq_membro;
    arq_membro = fopen("example.txt", "r");
    if(!arq_membro){
        printf("Falha ao abrir arquivo de leitura");
        return 1;
    }
    struct stat dados;
    stat("example.txt", &dados);
    for (long int i = 0; i < dados.st_size; i += BUFFER_SIZE)
    {
        fread(buffer, , 1, arq_membro);

    }
    
    while ()
    {
        fwrite(buffer, dados.st_size, 1, test);
    }
    return 0;
}