#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "lista.h"

#define BUFFER_SIZE 1024

int main() {
    Lista *l;
    l = cria_lista();

    int *x = (int *) malloc(sizeof(int));
    int *y = (int *) malloc(sizeof(int));
    int *z = (int *) malloc(sizeof(int));
    *x = 1;
    *y = 2;
    *z = 3;
    adiciona_final_lista(l, x);
    adiciona_final_lista(l, y);
    adiciona_final_lista(l, z);

    int dado;
    Nodo *nodo;
    nodo = l->head;
    while (nodo)
    {
        dado = *((int *) nodo->dado);
        printf("%d\n", dado);
        
        nodo = nodo->proximo;
    }

    remove_lista(l, z);

    nodo = l->head;
    while (nodo)
    {
        dado = *((int *) nodo->dado);
        printf("%d\n", dado);
        
        nodo = nodo->proximo;
    }
    
    
    

    return 0;
}