#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "vina.h"

void error_handler(Return_value value){
    switch (value)
    {
    case ERRO_ABRIR_ARCHIVE:
        fprintf(stderr, "Erro ao abrir/criar arquivo indicado como archive");
        exit(1);
        break;

    case ERRO_ABRIR_MEMBRO:
        fprintf(stderr, "Erro ao abrir arquivo indicado como membro");
        exit(1);
    
    default:
        break;
    }
}

int main(int argc, char **argv){
    int flag_i = 0;
    int flag_a = 0;
    int flag_m = 0;
    int flag_x = 0;
    int flag_r = 0;
    int flag_c = 0;
    int flag_h = 0;
    int option;

    opterr = 0;

    while ((option = getopt(argc, argv, "iamxrch:")) != -1)
    {
        switch (option)
        {
        case 'i':
            flag_i = 1;
            break;

        case 'a':
            flag_a = 1;
            break;

        case 'm':
            flag_m = 1;
            break;

        case 'x':
            flag_x = 1;
            break;

        case 'r':
            flag_r = 1;
            break;

        case 'c':
            flag_c = 1;
            break;

        case 'h':
            flag_h = 1;
            break;
            
        default:
            break;
        }

    }

    if(flag_i && argc >= 4){
        Archive *archive;
        archive = cria_archive();
        error_handler(inicia_archive(argv[2], archive));
        for (int i = 3; i <= argc - 1; i++)
        {
            printf("oi");
            error_handler(incluir(archive, argv[i]));
        }
        fseek(archive->archive_vpp, 0, SEEK_SET);
        int pos;
        fread(&pos, sizeof(int), 1, archive->archive_vpp);
        char mem[8];
        fread(mem, sizeof(char)*8, 1, archive->archive_vpp);
        printf("pos: %d - mem: %s", pos, mem);
    }

    
    
}