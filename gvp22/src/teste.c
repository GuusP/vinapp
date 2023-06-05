#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lista.h"

int main(){
    lista *l;
    if(!(l = cria_lista()))
        printf("Erro ao criar lista");

    int x = 23;
    if(!adiciona_final_lista(l, &x))
        printf("Erro ao adicionar na lista");

    int *ptr;
    ptr = l->head->dado;
    printf("%d", *ptr);
    
    return 0;
}