#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "vina.h"

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    FILE *test;
    test = fopen("test_file", "r");
    Nodo *nodo_membro;
    Lista *l;
    l = cria_lista();
    Membro m;
    /*m.location = 134;
    adiciona_final_lista(l, &m);
    nodo_membro = l->head;
    fwrite(nodo_membro->dado, sizeof(Membro), 1, test);*/
    fread(&m, sizeof(Membro), 1, test);
    printf("location: %u\n", m.location);
    

    return 0;
}