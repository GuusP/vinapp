/*
TODO: Remover prints desnecessários
      Comentar código
      Mudar a função sobreescrever para usar a função copiar
      Desalocar memória
*/

#include "vina.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>

#define BUFFER_SIZE 1024
#define TAM_NOME_DIR 256

Membro *cria_membro()
{
    Membro *novo_membro;

    if (!(novo_membro = malloc(sizeof(Membro))))
        return NULL;

    return novo_membro;
}

void salvar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive)
{
    Nodo *nodo_membro;

    nodo_membro = diretorio->membros->head;
    fseek(archive, inicio_dir + sizeof(int), SEEK_SET); // pula 4 bytes que serão usados para armazenar o tamanho do diretorio
    while (nodo_membro != NULL) // salva cada membro
    {
        fwrite(nodo_membro->dado, sizeof(Membro), 1, archive);
        nodo_membro = nodo_membro->proximo;
    }

    int final_dir = ftell(archive);
    diretorio->tamanho = final_dir - inicio_dir;
    fseek(archive, inicio_dir, SEEK_SET);
    fwrite(&(diretorio->tamanho), sizeof(int), 1, archive);
    fseek(archive, final_dir, SEEK_SET);
}

void carregar_diretorio(Diretorio *diretorio, int inicio_dir, FILE *archive)
{
    Membro *membro_buffer;
    membro_buffer = cria_membro();

    printf("inicio_dir: %d\n", inicio_dir);
    fseek(archive, inicio_dir, SEEK_SET);
    fread(&(diretorio->tamanho), sizeof(int), 1, archive);
    while (fread(membro_buffer, sizeof(Membro), 1, archive) == 1)
    {
        printf("lendo membro aqui\n");
        adiciona_final_lista(diretorio->membros, membro_buffer);
        membro_buffer = cria_membro();
    }
}

Diretorio *criar_diretorio()
{
    Diretorio *diretorio;

    if (!(diretorio = malloc(sizeof(Diretorio))))
        return NULL;

    if (!(diretorio->membros = cria_lista()))
    {
        free(diretorio);
        return NULL;
    }

    diretorio->tamanho = 0;

    return diretorio;
}

Archive *cria_archive()
{
    Archive *novo_archive;

    if (!(novo_archive = malloc(sizeof(Archive))))
        return NULL;

    novo_archive->archive_vpp = NULL;
    novo_archive->dir_vina = NULL;
    novo_archive->tamanho = 0;
    novo_archive->inicio_dir = sizeof(int);

    return novo_archive;
}

Return_value inicia_archive(char *caminho_archive, Archive *archive)
{
    archive->archive_vpp = fopen(caminho_archive, "r+");
    if (!archive->archive_vpp)
    {
        archive->archive_vpp = fopen(caminho_archive, "w+"); // como o archive não existia, cria um
        if (!archive->archive_vpp)
            return ERRO_ABRIR_ARCHIVE;
    }

    strcpy(archive->name, caminho_archive);
    struct stat propriedades;
    stat(caminho_archive, &propriedades);
    archive->tamanho = propriedades.st_size;
    fread(&(archive->inicio_dir), sizeof(int), 1, archive->archive_vpp);
    archive->dir_vina = criar_diretorio();
    carregar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);

    return SUCESSO;
}

// retorna o membro se encontrado e null se não encontrado
Membro *busca_membro(Diretorio *diretorio, char *caminho)
{
    printf("buscando...\n");
    Nodo *nodo_membro;
    nodo_membro = diretorio->membros->head;
    while (nodo_membro != NULL)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        printf("%s - %s\n", caminho, membro->name);
        if (strcmp(caminho, membro->name) == 0)
            return membro;

        nodo_membro = nodo_membro->proximo;
    }

    return NULL;
}

Membro *retorna_membro(Diretorio *diretorio, int membro_order)
{
    Nodo *nodo_membro;
    nodo_membro = diretorio->membros->head;
    while (nodo_membro != NULL && membro_order < diretorio->membros->quantidade)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        if (membro->order == membro_order)
            return membro;

        nodo_membro = nodo_membro->proximo;
    }

    return NULL;
}

int sobreescrever(FILE *archive, int tamanho, int posicao_leitura, int posicao_escrita)
{
    char buffer[BUFFER_SIZE];
    int bytes_restantes = tamanho;
    while (bytes_restantes > 0)
    {
        fseek(archive, posicao_leitura, SEEK_SET);
        if (bytes_restantes < BUFFER_SIZE)
        {
            fread(buffer, bytes_restantes, 1, archive);
            fseek(archive, posicao_escrita, SEEK_SET);
            fwrite(buffer, bytes_restantes, 1, archive);
            bytes_restantes = 0;
            posicao_escrita = ftell(archive);
        }
        else
        {
            fread(buffer, BUFFER_SIZE, 1, archive);
            posicao_leitura = ftell(archive);
            fseek(archive, posicao_escrita, SEEK_SET);
            fwrite(buffer, BUFFER_SIZE, 1, archive);
            bytes_restantes -= BUFFER_SIZE;
            posicao_escrita = ftell(archive);
        }
    }

    return posicao_escrita;
}

int remove_conteudo(Archive *archive, Membro *membro)
{
    int posicao_escrita = membro->position;
    Membro *proximo_membro;
    proximo_membro = retorna_membro(archive->dir_vina, membro->order + 1);

    while (proximo_membro != NULL)
    {
        int nova_pos = posicao_escrita;
        posicao_escrita = sobreescrever(archive->archive_vpp, proximo_membro->size, proximo_membro->position, posicao_escrita);
        proximo_membro->position = nova_pos;
        proximo_membro->order--;
        proximo_membro = retorna_membro(archive->dir_vina, proximo_membro->order + 2);
    }

    return posicao_escrita;
}

Return_value remocao(Archive *archive, Membro *membro)
{
    if (membro)
    {
        archive->inicio_dir = remove_conteudo(archive, membro);
        remove_lista(archive->dir_vina->membros, membro);
        salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
        if (ftruncate(fileno(archive->archive_vpp), ftell(archive->archive_vpp)))
            return ERRO_TRUNCAR;

        fseek(archive->archive_vpp, 0, SEEK_SET);
        fwrite(&archive->inicio_dir, sizeof(int), 1, archive->archive_vpp);

        return SUCESSO;
    }

    return MEMBRO_NAO_ENCONTRADO;
}

/*Copia tam bytes de source para dest. A escrita e leitura são realizadas nas posições atuais
dos ponteiros de cada arquivo.*/
void copiar(FILE *source, FILE *dest, int tam)
{
    char buffer[BUFFER_SIZE];
    int bytes_restantes = tam;
    while (bytes_restantes > 0)
    {
        if (bytes_restantes < BUFFER_SIZE)
        {
            fread(buffer, bytes_restantes, 1, source);
            fwrite(buffer, bytes_restantes, 1, dest);
            bytes_restantes = 0;
        }
        else
        {
            fread(buffer, BUFFER_SIZE, 1, source);
            fwrite(buffer, BUFFER_SIZE, 1, dest);
            bytes_restantes -= BUFFER_SIZE;
        }
    }
}

Return_value mover(Archive *archive, Membro *target, Membro *membro)
{
    int pos_escrita;

    if (!target)
        return TARGET_NAO_ENCONTRADO;

    if (!membro)
        return MEMBRO_NAO_ENCONTRADO;


    pos_escrita = target->position + target->size;
    ftruncate(fileno(archive->archive_vpp), archive->tamanho + membro->size); // aumenta o tamanho do arquivo para abrir espaço para a copia do membro na nova posição
    Membro *aux;

    for (int i = archive->dir_vina->membros->quantidade - 1; i > target->order; i--)
    {
        aux = retorna_membro(archive->dir_vina, i);
        for (int j = aux->position + aux->size - 1; j >= aux->position; j--)
        {
            /* move o conteúdo de todos os membros após o target para abrir espaço para mvoer o membro
               é feito de trás para frente para não sobrescrever nenhum byte
            */
            sobreescrever(archive->archive_vpp, sizeof(char), j, j + membro->size);
        }

        aux->position += membro->size;
    }
    archive->inicio_dir += membro->size;

    if (membro->order > target->order) // se move para trás
    {

        sobreescrever(archive->archive_vpp, membro->size, membro->position, pos_escrita); // move o conteúdo (bytes do membro) para a nova posição
        for (int i = membro->order - 1; i > target->order; i--)                           // aumenta a ordem dos membros que vem depois do membro na nova posição
        {
            Membro *mem_atual = retorna_membro(archive->dir_vina, i);
            mem_atual->order++;
            printf("%s: %d\n", mem_atual->name, mem_atual->order);
        }

        archive->inicio_dir = remove_conteudo(archive, membro); // remove a copia do membro que está na posição antiga
        membro->order = target->order + 1;
        membro->position = pos_escrita;
    }
    else
    {
        /*Se for mover para frente, cria uma cópia do membro (aux) para ter os metadados do conteúdo do membro
          na posição original e conseguir remover
        */
        Membro *aux;
        aux = cria_membro();
        aux->position = membro->position;
        aux->order = membro->order;
        aux->size = membro->size;
        sobreescrever(archive->archive_vpp, membro->size, membro->position, pos_escrita); // move para a nova posição
        for (int i = archive->dir_vina->membros->quantidade - 1; i > target->order; i--)
        {
            Membro *mem_atual = retorna_membro(archive->dir_vina, i);
            mem_atual->order++;
        }
        membro->position = pos_escrita;
        membro->order = target->order + 1;
        archive->dir_vina->membros->quantidade++;            // "adiciona" a copia do membro na nova posição no diretorio (função retorna_membro usada em remove_conteudo depende do valor de quantidade)
        archive->inicio_dir = remove_conteudo(archive, aux); // remove o conteúdo que estava na posição original
        archive->dir_vina->membros->quantidade--;            // "remove" o conteudo do membro na posição original do diretorio

        free(aux);
    }

    salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
    if (ftruncate(fileno(archive->archive_vpp), ftell(archive->archive_vpp))) // remove espaço adicional restante
        return ERRO_TRUNCAR;

    return SUCESSO;
}

Return_value incluir(Archive *archive, char *caminho_membro, int flag_a)
{
    FILE *arq_membro;
    struct stat dados;
    Membro *novo_membro;
    Membro *membro_existente;

    arq_membro = fopen(caminho_membro, "r");
    if (!arq_membro)
        return ERRO_ABRIR_MEMBRO;

    fseek(archive->archive_vpp, archive->inicio_dir, SEEK_SET);

    novo_membro = cria_membro();
    novo_membro->name[0] = '.';
    novo_membro->name[1] = '/';
    strcpy(&novo_membro->name[2], caminho_membro);

    stat(caminho_membro, &dados);
    novo_membro->position = ftell(archive->archive_vpp);
    novo_membro->size = dados.st_size;
    novo_membro->uid = dados.st_uid;
    novo_membro->mode = dados.st_mode;
    novo_membro->mtime = dados.st_mtime;

    membro_existente = busca_membro(archive->dir_vina, novo_membro->name);
    if (flag_a && membro_existente && membro_existente->mtime > novo_membro->mtime)
        return ARQUIVO_ANTIGO;

    copiar(arq_membro, archive->archive_vpp, dados.st_size);

    archive->inicio_dir = ftell(archive->archive_vpp);
    novo_membro->order = archive->dir_vina->membros->quantidade;
    adiciona_final_lista(archive->dir_vina->membros, novo_membro);
    salvar_diretorio(archive->dir_vina, archive->inicio_dir, archive->archive_vpp);
    if (membro_existente)
    {
        Membro *target;
        if (membro_existente->order > 0)
            target = retorna_membro(archive->dir_vina, membro_existente->order - 1);
        else
            target = membro_existente;

        mover(archive, target, novo_membro);
        remocao(archive, membro_existente);
    }
    else
    {
        int pos_ponteiro = ftell(archive->archive_vpp);
        fclose(archive->archive_vpp);
        truncate(archive->name, pos_ponteiro);
        archive->archive_vpp = fopen(archive->name, "r+");
        fwrite(&archive->inicio_dir, sizeof(int), 1, archive->archive_vpp);
    }

    return SUCESSO;
}

void extrai_membro(Archive *archive, Membro *membro, char *dir_atual)
{

    if(!membro)
        return;

    FILE *arq_membro;
    size_t length = strlen(membro->name);
    int pos = 0;
    for (int i = 0; i <= length; i++)
    { // pega o nome dos diretórios do arquivo e entra neles se existirem ou cria se não
        if (membro->name[i] == '/')
        {
            int count = i - pos;
            char aux[count + 1];
            strncpy(aux, &membro->name[pos], count);
            aux[count] = '\0';
            if (chdir(aux) == -1)
            {
                mkdir(aux, 0777);
                chdir(aux);
            }

            pos = i + 1;
        }
        else if (membro->name[i] == '\0')
        { // cria o arquivo extraído
            arq_membro = fopen(&membro->name[pos], "w");
            fseek(archive->archive_vpp, membro->position, SEEK_SET);
            copiar(archive->archive_vpp, arq_membro, membro->size);
            fclose(arq_membro);
            chmod(membro->name, membro->mode);
        }
    }

    chdir(dir_atual);
}

Return_value extrair(Archive *archive, char *caminho_membro)
{
    Membro *membro;
    char dir_atual[TAM_NOME_DIR];

    if (!getcwd(dir_atual, TAM_NOME_DIR))
        return TAMANHO_NOME_EXCEDIDO;

    if (!caminho_membro)
    { // extrai todos os membros do archive
        int order = 0;
        while ((membro = retorna_membro(archive->dir_vina, order)))
        {
            extrai_membro(archive, membro, dir_atual);
            order++;
        }
    }
    else
    { // extrai o membro indicado
        if ((membro = busca_membro(archive->dir_vina, caminho_membro)))
            extrai_membro(archive, membro, dir_atual);
        else
            return MEMBRO_NAO_ENCONTRADO;
    }

    return SUCESSO;
}

void lista_conteudo(Archive *archive)
{
    Nodo *nodo_membro;
    struct passwd *pw;

    nodo_membro = archive->dir_vina->membros->head;
    while (nodo_membro)
    {
        Membro *membro = (Membro *)nodo_membro->dado;
        printf("%d ", membro->order);
        // Permissões dono
        printf((membro->mode & S_IRUSR) ? "r" : "-");
        printf((membro->mode & S_IWUSR) ? "w" : "-");
        printf((membro->mode & S_IXUSR) ? "x" : "-");

        // Permissões grupo
        printf((membro->mode & S_IRGRP) ? "r" : "-");
        printf((membro->mode & S_IWGRP) ? "w" : "-");
        printf((membro->mode & S_IXGRP) ? "x" : "-");

        // Permissões outros
        printf((membro->mode & S_IROTH) ? "r" : "-");
        printf((membro->mode & S_IWOTH) ? "w" : "-");
        printf((membro->mode & S_IXOTH) ? "x " : "- ");

        if ((pw = getpwuid(membro->uid))) // coleta infos sobre o user com uid
            printf("%s ", pw->pw_name);

        printf("%ld ", membro->size);
        struct tm *time_info = localtime(&membro->mtime); // converte o tempo para o formato local

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info); // converte o tempo para um formato legivel

        printf("%s ", buffer);
        printf("%s\n", membro->name);

        nodo_membro = nodo_membro->proximo;
    }
}

void imprime_ajuda(){
    printf("-i: insere/acrescenta um ou mais membros ao archive. Caso o membro ja exista no archive, ele sera substituido.\nNovos membros são inseridos respeitando a ordem da linha de comando, ao final do archive;\n");
    printf("-a: mesmo comportamento da opcao -i, mas a substituicao de um membro existente ocorre APENAS caso o parametro\n seja *mais recente* que o arquivado\n");
    printf("-m target: move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive.\n");
    printf("-x: extrai os membros indicados de archive. Se os membros nao forem indicados, todos serao extraidos.\n");
    printf("-r: remove os membros indicados de archive.\n");
    printf("-c: lista o conteudo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissoes, tamanho e data de modificacao) e sua ordem no arquivo.\n");
    printf("-h: imprime uma pequena mensagem de ajuda com as opcoes disponiveis e encerra.\n");
    printf("Exemplo de insercao: vina++ -i backup.vpp arq.txt foto.jpg despesas.ods\n");

}