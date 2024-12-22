# Variables
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = main
OBJS = main.o avl.o

# Règle par défaut : compilation complète
all: $(TARGET)

# Création de l'exécutable principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilation des fichiers source en objets
main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

avl_tree.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)

# Règle pour tester rapidement
run: all
	./$(TARGET)

