# Vina++
O objetivo deste trabalho foi implementar o programa vina++, que consiste de um arquivador básico (archiver), isto é, um programa que salva em sequência uma coleção de arquivos (denominados membros) dentro de outro arquivo (denominado archive) cuja estrutura permite recuperar os arquivos originais individualmente.

Os programas tar, zip, rar e arj são arquivadores populares. Além de arquivar arquivos, a maioria deles também comprime os dados dos mesmos, para ocupar menos espaço de armazenamento. Para simplificar, o arquivador VINA++ não comprime os arquivos, apenas os armazena.

# Execução do Programa
Para executar o programa, acesse o diretório src e execute o Makefile. Após ser gerado o executável, execute o programa da seguinte forma:

./vina++ <opção> <archive> [membro1 membro2 ...]

Onde a opção pode ser:

    -i : insere/acrescenta um ou mais membros ao archive. Caso o membro já exista no archive, ele é substituído. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive;
    -a : mesmo comportamento da opção -i, mas a substituição de um membro existente ocorre APENAS caso o parâmetro seja mais recente que o arquivado;
    -m target : move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive.
    -x : extrai os membros indicados de archive. Se os membros não forem indicados, todos são extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco;
    -r : remove os membros indicados de archive;
    -c : lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissões, tamanho e data de modificação) e sua ordem no arquivo. A saída é igual ao do comando tar com as opções tvf
    -h : imprime uma pequena mensagem de ajuda com as opções disponíveis e encerra.

Caso sejam indicados nomes de arquivos com caminhos absolutos ou relativos, arquiva-se estes nomes SEMPRE com caminhos relativos. Por exemplo, se for indicado o arquivo /home/inf/xyz00/texto.doc, é colocado no arquivo vpp como ./home/inf/xyz00/texto.doc.

Ao extrair um membro, toda a hierarquia de diretórios contidos no nome do arquivo é criada (caso não existam). 

Para checar o enunciado completo do trabalho, acesse: https://wiki.inf.ufpr.br/maziero/doku.php?id=prog2:vinapp
Para uma explicação das funções e algoritmos usados, leia o arquivo src/Documentação
