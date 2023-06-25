#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "lista.h"
#include <time.h>

struct Membro{
    char name[256];
    uid_t uid; // user id
    mode_t mode; // permissões
    off_t size; // tamanho em bytes
    time_t mtime; // tempo de modificação
    unsigned int position; // posicao no arquivo em bytes
    unsigned int order; // ordem no arquvio
} typedef Membro;

struct Diretorio{
    Lista *membros;
    int tamanho;
} typedef Diretorio;

struct Archive{
    Diretorio *dir_vina;
    int inicio_dir;
    int tamanho;
    FILE *archive_vpp;
    char name[256];
}typedef Archive;

typedef enum { SUCESSO, ERRO_ABRIR_ARCHIVE, ERRO_ABRIR_MEMBRO, ERRO_TRUNCAR, MEMBRO_NAO_ENCONTRADO, TARGET_NAO_ENCONTRADO, TAMANHO_NOME_EXCEDIDO, ORDEM_IGUAL} Return_value;

Return_value incluir(Archive *archive, char *caminho_membro);

Return_value remocao(Archive *archive, char *caminho_membro);

Return_value mover(Archive *archive, char *caminho_target, char *caminho_membro);

Return_value extrair(Archive *archive, char *caminho_membro);

int salvar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive);

int carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive);

Return_value inicia_archive(char *caminho_archive, Archive *archive);

Archive *cria_archive();

void lista_conteudo(Archive *archive);
