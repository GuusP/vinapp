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
    struct timespec tempo; 
    unsigned int position; // posicao no arquivo
    unsigned int location; // localização
} typedef Membro;

struct Diretorio{
    Lista *membros;

} typedef Diretorio;

struct Archive{
    Diretorio *dir_vina;
    int inicio_dir;
    int tamanho;
    FILE *archive_vpp;
}typedef Archive;

typedef enum { SUCESSO, ERRO_ABRIR_ARCHIVE, ERRO_ABRIR_MEMBRO } Return_value;

Return_value incluir(Archive *archive, Diretorio *diretorio, char *caminho_membro);