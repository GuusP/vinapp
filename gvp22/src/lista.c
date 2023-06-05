#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

/*
Cria um nodo da lista. Retorna o endereço em caso de sucesso e NULL
caso falhe
*/
nodo *cria_nodo_lista(nodo *anterior, nodo *prox, void *valor){
    nodo *nodo_novo;

    if(!(nodo_novo = malloc(sizeof(nodo))))
        return NULL;

    nodo_novo->anterior = anterior;
    nodo_novo->proximo = prox;
    nodo_novo->dado = valor;

    return nodo_novo;
}

/*
Cria uma lista vazia e retorna o endereço dela em caso de
sucesso e NULL caso falhe
*/
lista *cria_lista(){
    lista *nova_lista;

    if(!(nova_lista = malloc(sizeof(lista))))
        return NULL;

    nova_lista->head = NULL;
    nova_lista->quantidade = 0;
    return nova_lista;    
}

/*
Destroi uma lista e retorna NULL
*/
lista *destroi_lista(lista *l){
    if(!l)
        return NULL;

    while (l->head)
    {
        nodo *prox = l->head->proximo;
        l->head->proximo = NULL;
        free(l->head);
        l->head = prox;
    }

    free(l);
    return NULL;
}

/*
Adiciona um elemento na primeira posição da lista. Retorna 1
caso de certo e 0 em caso de falha
*/
int adiciona_inicio_lista(lista *l, void *valor){
    if(!l)
        return 0;

    nodo *novo_nodo;
    if(!(novo_nodo = cria_nodo_lista(NULL, l->head, valor)))
        return 0;

    l->head = novo_nodo;
    l->quantidade++;

    return 1;
}

/*
Adiciona um elemento no final da lista. Retorna 1 caso de
certo e 0 em caso de falha
*/
int adiciona_final_lista(lista *l, void *valor){
    if(!l)
        return 0;

    nodo *novo_nodo;
    if(!(novo_nodo = cria_nodo_lista(NULL, NULL, valor)))
        return 0;

    
    if(l->head){
        nodo *ultimo_nodo;
        ultimo_nodo = l->head;
        while (ultimo_nodo->proximo)
            ultimo_nodo = ultimo_nodo->proximo;

        novo_nodo->anterior = ultimo_nodo;
        ultimo_nodo->proximo = novo_nodo;
    }else{
        l->head = novo_nodo;
    }
    
    l->quantidade++;
    

    return 1;
}

/*
Retorna o endereço do primeiro elemento da lista
*/
nodo *obtem_primeiro_lista(lista *l){
    if(!l)
        return NULL;

    return l->head;
}

/*
Retorna a quantidade de elementos na lista
*/
int count(lista *l){
    if(!l)
        return 0;

    return l->quantidade;
}
