typedef struct nodo{
    void *dado;
    struct nodo *proximo;
    struct nodo *anterior;
} nodo;

typedef struct lista{
    nodo *head;
    int quantidade;
} lista;

nodo *cria_nodo_lista(nodo *anterior, nodo *prox, void *valor);

lista *cria_lista();

lista *destroi_lista(lista *l);

int adiciona_inicio_lista(lista *l, void *valor);

int adiciona_final_lista(lista *l, void *valor);

nodo *obtem_primeiro_lista(lista *l);

int count(lista *l);