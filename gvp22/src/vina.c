#include "vina.h"

#define BUFFER_SIZE 1024

int incluir(FILE *arq, Archive archive, Diretorio diretorio, char *caminho_membro){
    FILE *arq_membro;
    
    arq_membro = fopen("r", caminho_membro);
    if(!arq_membro)
        return 0;

    fseek(arq, archive.inicio_dir, SEEK_SET);
    char buffer[BUFFER_SIZE];
    FILE *test;
    test = fopen("./test_file");
    while (fread(buffer, BUFFER_SIZE, 1, arq_membro) == BUFFER_SIZE);
    {
        fwrite()
    }
    
}