typedef struct Nodo{
    void *dado;
    struct Nodo *proximo;
    struct Nodo *anterior;
} Nodo;

typedef struct Lista{
    Nodo *head;
    int quantidade;
} Lista;

Nodo *cria_nodo_lista(Nodo *anterior, Nodo *prox, void *valor);

Lista *cria_lista();

Lista *destroi_lista(Lista *l);

int adiciona_inicio_lista(Lista *l, void *valor);

int adiciona_final_lista(Lista *l, void *valor);

Nodo *obtem_primeiro_lista(Lista *l);

int count(Lista *l);