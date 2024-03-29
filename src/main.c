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
        fprintf(stderr, "Erro ao abrir/criar arquivo indicado como archive\n");
        exit(1);
        break;

    case ERRO_ABRIR_MEMBRO:
        fprintf(stderr, "Erro ao abrir arquivo indicado como membro\n");
        exit(1);

    case ERRO_TRUNCAR:
        fprintf(stderr, "Erro ao truncar arquivo indicado com archive\n");
        exit(1);

    case MEMBRO_NAO_ENCONTRADO:
        fprintf(stderr, "Um membro indicado nao foi encontrado\n");

    case TAMANHO_NOME_EXCEDIDO:
        fprintf(stderr, "Tamnho do nome do diretorio atual ou caminho para membro muito grande\n");
        exit(1);

    case ORDEM_IGUAL:
        fprintf(stderr, "O membro indicado ja esta logo apos ao target\n");
        exit(1);

    case ARQUIVO_ANTIGO:
        fprintf(stderr, "A versao do arquivo passado eh mais antiga que a ja existente no archive\n");
        exit(1);

    case TARGET_NAO_ENCONTRADO:
        fprintf(stderr, "Target nao encontrado\n");
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

    if (flag_c)
    {
        Archive *archive;
        archive = cria_archive();
        error_handler(inicia_archive(argv[2], archive));
        lista_conteudo(archive);
    }

    if (flag_h)
    {
        imprime_ajuda();
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
                error_handler(incluir(archive, argv[i], 0));
            }
        }
        else if (flag_a)
        {
            for (int i = 3; i <= argc - 1; i++)
            {
                error_handler(incluir(archive, argv[i], 1));
            }
        }
        else if (flag_r)
        {
            for (int i = 3; i <= argc - 1; i++)
            {
                int tamanho_nome = sizeof(argv[i]);
                char nome[tamanho_nome + 2];
                nome[0] = '.';
                nome[1] = '/';
                strcpy(&(nome[2]), argv[i]);
                error_handler(remocao(archive, busca_membro(archive->dir_vina, nome)));
            }
        }
        else if (flag_x)
        {
            for (int i = 3; i <= argc - 1; i++)
            {
                int tamanho_nome = sizeof(argv[i]);
                char nome[tamanho_nome + 2];
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
            int tamanho_nome = sizeof(argv[2]);
            char target[tamanho_nome + 2];
            target[0] = '.';
            target[1] = '/';
            strcpy(&(target[2]), argv[2]);
            tamanho_nome = sizeof(argv[4]);
            char membro[tamanho_nome + 2];
            membro[0] = '.';
            membro[1] = '/';
            strcpy(&(membro[2]), argv[4]);
            mover(archive, busca_membro(archive->dir_vina, target), busca_membro(archive->dir_vina, membro));
        }
    }
}