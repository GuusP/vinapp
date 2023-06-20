#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "vina.h"

void error_handler(Return_value value)
{
    switch (value)
    {
    case ERRO_ABRIR_ARCHIVE:
        fprintf(stderr, "Erro ao abrir/criar arquivo indicado como archive");
        exit(1);
        break;

    case ERRO_ABRIR_MEMBRO:
        fprintf(stderr, "Erro ao abrir arquivo indicado como membro");
        exit(1);

    case ERRO_TRUNCAR:
        fprintf(stderr, "Erro ao truncar arquivo indicado com archive");
        exit(1);

    case MEMBRO_NAO_ENCONTRADO:
        fprintf(stderr, "Um membro indicado nao foi encontrado");

    case TAMANHO_NOME_EXCEDIDO:
        fprintf(stderr, "Tamnho do nome do diretorio atual ou caminho para membro muito grande");
        exit(1);

    default:
        break;
    }
}

int main(int argc, char **argv)
{
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

    if (argc == 3)
    {
        if (flag_x)
        {
            Archive *archive;
            archive = cria_archive();
            error_handler(inicia_archive(argv[2], archive));
            extrair(archive, NULL);
        }
    }

    if (argc >= 4)
    {
        Archive *archive;
        archive = cria_archive();
        error_handler(inicia_archive(argv[2], archive));

        if (flag_i)
        {

            for (int i = 3; i <= argc - 1; i++)
            {
                int tamanho_nome = sizeof(argv[i]);
                char nome[tamanho_nome];
                nome[0] = '.';
                nome[1] = '/';
                strcpy(&(nome[2]), argv[i]);
                error_handler(incluir(archive, nome));
            }
        }
        else if (flag_a)
        {
            printf("flag_a n foi implementada");
        }
        else if (flag_r)
        {
            for (int i = 3; i <= argc - 1; i++)
            {
                int tamanho_nome = sizeof(argv[i]);
                char nome[tamanho_nome];
                nome[0] = '.';
                nome[1] = '/';
                strcpy(&(nome[2]), argv[i]);
                error_handler(remocao(archive, nome));
            }
        }
        else if (flag_x)
        {
            for (int i = 3; i <= argc - 1; i++)
            {
                int tamanho_nome = sizeof(argv[i]);
                char nome[tamanho_nome];
                nome[0] = '.';
                nome[1] = '/';
                strcpy(&(nome[2]), argv[i]);
                extrair(archive, nome);
            }
        }
    }

    if (argc >= 5)
    {
        if (flag_m)
        {
            Archive *archive;
            archive = cria_archive();
            error_handler(inicia_archive(argv[3], archive));

            mover(archive, argv[2], argv[4]);
        }
    }
}