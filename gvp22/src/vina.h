#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct membro{
    char name[256];
    uid_t uid; // user id
    mode_t mode; // permissões
    off_t size; // tamanho em bytes
    struct timespec st_mtim; 
    unsigned int order; // ordem no arquivo
    unsigned int location; // localização
} typedef membro;

struct diretorio{
    membro *membros; // transformar numa lista

} typedef diretorio;