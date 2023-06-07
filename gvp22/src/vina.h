#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "lista.h"

struct Membro{
    char name[256];
    uid_t uid; // user id
    mode_t mode; // permissões
    off_t size; // tamanho em bytes
    struct timespec st_mtim; 
    unsigned int order; // ordem no arquivo
    unsigned int location; // localização
} typedef membro;

struct Diretorio{
    Lista membros;

} typedef Diretorio;

struct Archive{
    Diretorio dir_vina;
    int inicio_dir;
}typedef Archive;

int incluir(Archive archive, Diretorio diretorio, char *caminho_membro);