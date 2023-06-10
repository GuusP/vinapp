#include "vina.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int salvar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive){
    Nodo *nodo_membro;

    nodo_membro = diretorio->membros->head;
    fseek(archive, inicio_dir, SEEK_SET);
    while (nodo_membro != NULL)
    {
        fwrite(nodo_membro->dado, sizeof(Membro), 1, archive);
        nodo_membro = nodo_membro->proximo;
    }
    
}

int carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive){
    Membro membro_buffer;

    fseek(archive, inicio_dir, SEEK_SET);

    while (fread(&membro_buffer, sizeof(Membro), 1, archive) == 1){
        adiciona_final_lista(diretorio->membros, &membro_buffer);
        printf("oi");
    }
        
    
}

Diretorio *criar_diretorio(){
    Diretorio *diretorio;

    if(!(diretorio = malloc(sizeof(Diretorio))))
        return NULL;
    
    if(!(diretorio->membros = cria_lista())){
        free(diretorio);
        return NULL;
    }

    return diretorio;
}

Archive *cria_archive(){
    Archive *novo_archive;
    
    if(!(novo_archive = malloc(sizeof(Archive))))
        return NULL;

    novo_archive->archive_vpp = NULL;
    novo_archive->dir_vina = NULL;
    novo_archive->tamanho = 0;
    novo_archive->inicio_dir = sizeof(int);

    return novo_archive;
}

Return_value inicia_archive(char *caminho_archive, Archive *archive){
    archive->archive_vpp = fopen(caminho_archive, "r+");
    if(!archive->archive_vpp){
        archive->archive_vpp = fopen(caminho_archive, "w+"); // como o archive não existia, cria um
        if(!archive->archive_vpp)
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

Return_value incluir(Archive *archive, char *caminho_membro){
    char buffer[BUFFER_SIZE];
    FILE *arq_membro;
    struct stat dados;
    Membro novo_membro;

    arq_membro = fopen(caminho_membro, "r");
    if (!arq_membro)
        return ERRO_ABRIR_MEMBRO;

    fseek(archive->archive_vpp, archive->inicio_dir, SEEK_SET);

    novo_membro.name[0]= '.';
    novo_membro.name[1]= '/';
    strcpy(&novo_membro.name[2], caminho_membro);

    Nodo *nodo_membro;
    nodo_membro = archive->dir_vina->membros->head;
    while (nodo_membro != NULL)
    {
        
        Membro *membro = (Membro *)nodo_membro->dado;
        if(strcmp(novo_membro.name, membro->name))
            fseek(archive->archive_vpp, membro->position, SEEK_SET);

        nodo_membro = nodo_membro->proximo;
    }
    
    stat(caminho_membro, &dados);
    novo_membro.position = ftell(archive->archive_vpp);
    novo_membro.size = dados.st_size;

    int bytes_restantes = dados.st_size;
    while (bytes_restantes > 0) {
        int bytes_lidos;
        if (bytes_restantes < BUFFER_SIZE) {
            bytes_lidos = fread(buffer, 1, bytes_restantes, arq_membro);
            fwrite(buffer, 1, bytes_lidos, archive->archive_vpp);
            bytes_restantes = 0;
        } else {
            bytes_lidos = fread(buffer, 1, BUFFER_SIZE, arq_membro);
            fwrite(buffer, 1, bytes_lidos, archive->archive_vpp);
            bytes_restantes -= bytes_lidos;
        }
    }

    archive->inicio_dir = ftell(archive->archive_vpp);

    adiciona_final_lista(archive->dir_vina->membros, &novo_membro);
    salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
    int pos_ponteiro = ftell(archive->archive_vpp);
    fclose(archive->archive_vpp);
    truncate(archive->name, pos_ponteiro);
    archive->archive_vpp = fopen(archive->name, "r+");
    fwrite(&archive->inicio_dir, sizeof(int), 1, archive->archive_vpp);
    
    return SUCESSO;
}