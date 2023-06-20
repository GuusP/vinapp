#include "vina.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024
#define PATH_MAX 256

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
    int pos_atual = ftell(archive);
    while (nodo_membro != NULL) // salva cada membro
    {
        fwrite(nodo_membro->dado, sizeof(Membro), 1, archive);
        nodo_membro = nodo_membro->proximo;
    }

    int final_dir = ftell(archive);
    diretorio->tamanho = final_dir - inicio_dir;
    fseek(archive, inicio_dir, SEEK_SET);
    fwrite(&(diretorio->tamanho), sizeof(int), 1, archive);
    fseek(archive, final_dir, SEEK_SET);
}

int carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive)
{
    Membro *membro_buffer;
    membro_buffer = cria_membro();

    printf("inicio_dir: %d\n", inicio_dir);
    fseek(archive, inicio_dir, SEEK_SET);
    fread(&(diretorio->tamanho), sizeof(int), 1, archive);
    while (fread(membro_buffer, sizeof(Membro), 1, archive) == 1)
    {
        printf("lendo membro aqui\n");
        adiciona_final_lista(diretorio->membros, membro_buffer);
        membro_buffer = cria_membro();
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

// retorna o membro se encontrado e null se não encontrado
Membro *busca_membro(Diretorio *diretorio, char *caminho)
{
    printf("buscando...\n");
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
    while (nodo_membro != NULL && membro_order < diretorio->membros->quantidade)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        if (membro->order == membro_order)
            return membro;

        nodo_membro = nodo_membro->proximo;
    }

    return NULL;
}

/*TODO: Mudar a função sobreescrever para usar a função copiar*/
int sobreescrever(FILE *archive, int tamanho, int posicao_leitura, int posicao_escrita)
{
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
            posicao_escrita = ftell(archive);
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
        int posicao_escrita = membro->position;
        Membro *proximo_membro;
        proximo_membro = retorna_membro(archive->dir_vina, membro->order + 1);
        while (proximo_membro != NULL)
        {
            int nova_pos = posicao_escrita;
            posicao_escrita = sobreescrever(archive->archive_vpp, proximo_membro->size, proximo_membro->position, posicao_escrita);
            proximo_membro->position = nova_pos;
            proximo_membro->order--;
            proximo_membro = retorna_membro(archive->dir_vina, proximo_membro->order + 2);
        }

        remove_lista(archive->dir_vina->membros, membro);
        printf("pos_Esc: %d\n", posicao_escrita);
        archive->inicio_dir = posicao_escrita;
        salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
        if (ftruncate(fileno(archive->archive_vpp), ftell(archive->archive_vpp)))
            return ERRO_TRUNCAR;

        fseek(archive->archive_vpp, 0, SEEK_SET);
        fwrite(&archive->inicio_dir, sizeof(int), 1, archive->archive_vpp);

        return SUCESSO;
    }

    return MEMBRO_NAO_ENCONTRADO;
}

/*Copia tam bytes de source para dest. A escrita e leitura são realizadas nas posições atuais
dos ponteiros de cada arquivo.*/
void copiar(FILE *source, FILE *dest, int tam)
{
    char buffer[BUFFER_SIZE];
    int bytes_restantes = tam;
    while (bytes_restantes > 0)
    {
        if (bytes_restantes < BUFFER_SIZE)
        {
            fread(buffer, bytes_restantes, 1, source);
            fwrite(buffer, bytes_restantes, 1, dest);
            bytes_restantes = 0;
        }
        else
        {
            fread(buffer, BUFFER_SIZE, 1, source);
            fwrite(buffer, BUFFER_SIZE, 1, dest);
            bytes_restantes -= BUFFER_SIZE;
        }
    }
}

Return_value mover(Archive *archive, char *caminho_target, char *caminho_membro)
{
    Membro *target;
    Membro *membro;

    if (!(target = busca_membro(archive->dir_vina, caminho_target)))
        return TARGET_NAO_ENCONTRADO;

    if (!(membro = busca_membro(archive->dir_vina, caminho_membro)))
        ;
    return MEMBRO_NAO_ENCONTRADO;

    ftruncate(fileno(archive->archive_vpp), archive->tamanho + membro->size);
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

    stat(caminho_membro, &dados);
    novo_membro->position = ftell(archive->archive_vpp);
    novo_membro->size = dados.st_size;

    copiar(arq_membro, archive->archive_vpp, dados.st_size);

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

Return_value extrair(Archive *archive, char *caminho_membro)
{
    Membro *membro;
    FILE *arq_membro;
    char dir_atual[PATH_MAX];

    if(!getcwd(dir_atual, PATH_MAX))
        return TAMANHO_NOME_EXCEDIDO;

    if (!caminho_membro)
    { // extrai todos os membros do archive
        int order = 0;
        while (membro = retorna_membro(archive->dir_vina, order))
        {
            arq_membro = fopen(membro->name, "w");
            fseek(archive->archive_vpp, membro->position, SEEK_SET);
            copiar(archive->archive_vpp, arq_membro, membro->size);
            fclose(arq_membro);
            order++;
        }
    }
    else
    { // extrai o membro indicado 
        if ((membro = busca_membro(archive->dir_vina, caminho_membro)))
        {
            size_t length = strlen(membro->name);
            int pos = 0;
            for (int i = 0; i <= length; i++)
            { // pega o nome dos diretórios do arquivo e entra neles se existirem ou cria se não
                if (membro->name[i] == '/')
                {
                    int count = i - pos;
                    char aux[count + 1];
                    strncpy(aux, &membro->name[pos], count);
                    aux[count] = '\0';
                    if (chdir(aux) == -1)
                    {
                        mkdir(aux, 0777);
                        chdir(aux);
                    }

                    pos = i + 1;
                }
                else if (membro->name[i] == '\0')
                { // cria o arquivo extraído
                    arq_membro = fopen(&membro->name[pos], "w");
                    fseek(archive->archive_vpp, membro->position, SEEK_SET);
                    copiar(archive->archive_vpp, arq_membro, membro->size);
                    fclose(arq_membro);
                }
            }
        }
        else
        {
            return MEMBRO_NAO_ENCONTRADO;
        }
    }

    chdir(dir_atual);
    return SUCESSO;
}