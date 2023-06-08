#include "vina.h"

#define BUFFER_SIZE 1024

int salvar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive){
    Membro membro = diretorio->membros->head->dado;
    while (membro != NULL)
    {
        /* code */
    }
    
}

int carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive){
    Membro membro_buffer;

    fseek(archive, inicio_dir, SEEK_SET);

    while (fread(&membro_buffer, sizeof(Membro), 1, archive) == 1)
        adiciona_final_lista(diretorio->membros, &membro_buffer);
    

}

Return_value inicia_archive(char *caminho_archive, Archive *archive){
    archive->archive_vpp = fopen(caminho_archive, "r+");
    if(!archive->archive_vpp){
        archive->archive_vpp = fopen(caminho_archive, "w"); // como o archive não existia, cria um
        if(!archive->archive_vpp)
            return ERRO_ABRIR_ARCHIVE;
    }

    struct stat propriedades;
    stat(caminho_archive, &propriedades);
    archive->tamanho = propriedades.st_size;
    fread(&(archive->inicio_dir), sizeof(int), 1, archive->archive_vpp);
    carregar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
    return SUCESSO;
}

Return_value incluir(Archive *archive, Diretorio *diretorio, char *caminho_membro){
    char buffer[BUFFER_SIZE];
    FILE *arq_membro;
    struct stat dados;

    arq_membro = fopen(caminho_membro, "r");
    if (!arq_membro)
        return ERRO_ABRIR_MEMBRO;

    fseek(archive->inicio_dir, 1, SEEK_SET);
    stat(caminho_membro, &dados);
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
}