# Variáveis
CC = gcc
CFLAGS = -Wall 

# Diretórios
SRC_DIR = src
BIN_DIR = .

# Arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

# Nome do executável
EXEC = $(BIN_DIR)/vina++

# Regra principal
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Regra para compilar os arquivos fonte
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos temporários e gerados
clean:
	rm -f $(OBJS) $(EXEC)
