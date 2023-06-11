#include "vina.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

Membro *cria_membro()
{
    Membro *novo_membro;

    if (!(novo_membro = malloc(sizeof(Membro))))
        return NULL;

    return novo_membro;
}

int salvar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive)
{
    Nodo *nodo_membro;

    nodo_membro = diretorio->membros->head;
    fseek(archive, inicio_dir + sizeof(int), SEEK_SET); // pula 4 bytes que serão usados para armazenar o tamanho do diretorio
    while (nodo_membro != NULL) // salva cada membro
    {
        fwrite(nodo_membro->dado, sizeof(Membro), 1, archive);
        nodo_membro = nodo_membro->proximo;
    }

    diretorio->tamanho = ftell(archive) - inicio_dir;
    fseek(archive, inicio_dir, SEEK_SET);
    fwrite(&(diretorio->tamanho), sizeof(int), 1, archive);
    fseek(archive, inicio_dir + diretorio->tamanho, SEEK_SET);
}

int carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive)
{
    Membro *membro_buffer;
    membro_buffer = cria_membro();

    fseek(archive, inicio_dir, SEEK_SET);
    fread(&(diretorio->tamanho), sizeof(int), 1, archive);
    while (fread(membro_buffer, sizeof(Membro), 1, archive) == 1)
    {
        adiciona_final_lista(diretorio->membros, membro_buffer);
        printf("oi");
    }

    
}

Diretorio *criar_diretorio()
{
    Diretorio *diretorio;

    if (!(diretorio = malloc(sizeof(Diretorio))))
        return NULL;

    if (!(diretorio->membros = cria_lista()))
    {
        free(diretorio);
        return NULL;
    }

    diretorio->tamanho = 0;

    return diretorio;
}

Archive *cria_archive()
{
    Archive *novo_archive;

    if (!(novo_archive = malloc(sizeof(Archive))))
        return NULL;

    novo_archive->archive_vpp = NULL;
    novo_archive->dir_vina = NULL;
    novo_archive->tamanho = 0;
    novo_archive->inicio_dir = sizeof(int);

    return novo_archive;
}

Return_value inicia_archive(char *caminho_archive, Archive *archive)
{
    archive->archive_vpp = fopen(caminho_archive, "r+");
    if (!archive->archive_vpp)
    {
        archive->archive_vpp = fopen(caminho_archive, "w+"); // como o archive não existia, cria um
        if (!archive->archive_vpp)
            return ERRO_ABRIR_ARCHIVE;
    }

    strcpy(archive->name, caminho_archive);
    struct stat propriedades;
    stat(caminho_archive, &propriedades);
    archive->tamanho = propriedades.st_size;
    fread(&(archive->inicio_dir), sizeof(int), 1, archive->archive_vpp);
    archive->dir_vina = criar_diretorio();
    carregar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);

    return SUCESSO;
}


// retorna a posição em bytes do arquivo se encontrado e -1 caso não encontrado
Membro *busca_membro(Diretorio *diretorio, char *caminho)
{
    Nodo *nodo_membro;
    nodo_membro = diretorio->membros->head;
    while (nodo_membro != NULL)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        printf("%s - %s\n", caminho, membro->name);
        if (strcmp(caminho, membro->name) == 0)
            return membro;

        nodo_membro = nodo_membro->proximo;
    }

    return NULL;
}

Membro *retorna_membro(Diretorio *diretorio, int membro_order)
{
    Nodo *nodo_membro;
    nodo_membro = diretorio->membros->head;
    while (nodo_membro != NULL)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        if (membro->order == membro_order)
            return membro;

        nodo_membro = nodo_membro->proximo;
    }

    return NULL;
}

int sobreescrever(FILE *archive, int tamanho, int posicao_leitura, int posicao_escrita)
{
    printf("tamanho: %d - posicao_leitura: %d - posicao_escrita: %d\n", tamanho, posicao_leitura, posicao_escrita);
    char buffer[BUFFER_SIZE];
    int bytes_restantes = tamanho;
    while (bytes_restantes > 0)
    {
        fseek(archive, posicao_leitura, SEEK_SET);
        if (bytes_restantes < BUFFER_SIZE)
        {
            fread(buffer, bytes_restantes, 1, archive);
            fseek(archive, posicao_escrita, SEEK_SET);
            fwrite(buffer, bytes_restantes, 1, archive);
            bytes_restantes = 0;
        }
        else
        {
            fread(buffer, BUFFER_SIZE, 1, archive);
            posicao_leitura = ftell(archive);
            fseek(archive, posicao_escrita, SEEK_SET);
            fwrite(buffer, BUFFER_SIZE, 1, archive);
            bytes_restantes -= BUFFER_SIZE;
            posicao_escrita = ftell(archive);
        }
    }

    return posicao_escrita;
}

Return_value remocao(Archive *archive, char *caminho_membro)
{
    Membro *membro;

    if (membro = busca_membro(archive->dir_vina, caminho_membro))
    {
        printf("achei\n");
        if (membro->order + 1 == archive->dir_vina->membros->quantidade)
        {
            printf("eh o ultimo membro\n");
            sobreescrever(archive->archive_vpp, archive->dir_vina->tamanho, archive->inicio_dir, membro->position);
        }
        else
        {
            printf("n eh o ultimo membro\n");
            Membro *proximo_membro;
            proximo_membro = retorna_membro(archive->dir_vina, membro->order + 1);
            int posicao_escrita = membro->position;
            while (proximo_membro != NULL)
            {
                posicao_escrita = sobreescrever(archive->archive_vpp, proximo_membro->size, proximo_membro->position, posicao_escrita);

                proximo_membro = retorna_membro(archive->dir_vina, proximo_membro->order + 1);
            }
        }
    }
}

Return_value incluir(Archive *archive, char *caminho_membro)
{
    char buffer[BUFFER_SIZE];
    FILE *arq_membro;
    struct stat dados;
    Membro *novo_membro;

    arq_membro = fopen(caminho_membro, "r");
    if (!arq_membro)
        return ERRO_ABRIR_MEMBRO;

    fseek(archive->archive_vpp, archive->inicio_dir, SEEK_SET);

    novo_membro = cria_membro();
    strcpy(novo_membro->name, caminho_membro);

    Nodo *nodo_membro;
    nodo_membro = archive->dir_vina->membros->head;
    while (nodo_membro != NULL)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        if (strcmp(novo_membro->name, membro->name) == 0)
            fseek(archive->archive_vpp, membro->position, SEEK_SET);

        nodo_membro = nodo_membro->proximo;
    }

    stat(caminho_membro, &dados);
    novo_membro->position = ftell(archive->archive_vpp);
    novo_membro->size = dados.st_size;

    int bytes_restantes = dados.st_size;
    while (bytes_restantes > 0)
    {
        if (bytes_restantes < BUFFER_SIZE)
        {
            fread(buffer, bytes_restantes, 1, arq_membro);
            fwrite(buffer, bytes_restantes, 1, archive->archive_vpp);
            bytes_restantes = 0;
        }
        else
        {
            fread(buffer, BUFFER_SIZE, 1, arq_membro);
            fwrite(buffer, BUFFER_SIZE, 1, archive->archive_vpp);
            bytes_restantes -= BUFFER_SIZE;
        }
    }

    archive->inicio_dir = ftell(archive->archive_vpp);

    novo_membro->order = archive->dir_vina->membros->quantidade;
    adiciona_final_lista(archive->dir_vina->membros, novo_membro);
    salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
    int pos_ponteiro = ftell(archive->archive_vpp);
    fclose(archive->archive_vpp);
    truncate(archive->name, pos_ponteiro);
    archive->archive_vpp = fopen(archive->name, "r+");
    fwrite(&archive->inicio_dir, sizeof(int), 1, archive->archive_vpp);

    return SUCESSO;
}