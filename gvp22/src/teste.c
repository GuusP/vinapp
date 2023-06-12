#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "vina.h"

#define BUFFER_SIZE 1024

int main() {
    Lista *l;
    l = cria_lista();

    Membro *mem1;
    mem1 = malloc(sizeof(Membro));
    Membro *mem2;
    mem2 = malloc(sizeof(Membro));
    mem1->position = 1;
    mem2->position = 2;

    adiciona_final_lista(l, mem1);
    adiciona_final_lista(l, mem2);
    printf("mem1: %p - mem2: %p\n", mem1, mem2);
    remove_lista(l, mem1);
    printf("%p\n", l->head->dado);
    remove_lista(l, mem2);

    return 0;
}