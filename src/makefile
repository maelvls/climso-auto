#
# makefile
# Mael Valais, 2014-04-07 15:23
#
OPT=-g -lm
CC=clang++
BIN=a.out
BIN_DIR=bin
all:convol.o interpol.o crea_tiff_3.o fcts_LK3.o main.o
	$(CC) $^ -o $(BIN) $(OPT)
%.o: %.c
	$(CC) -o $@ -c $<
clean:
	rm -rf $(BIN) *.o

#$@     Le nom de la cible
#$<     Le nom de la première dépendance
#$^     La liste des dépendances
#$?     La liste des dépendances plus récentes que la cible
#$*     Le nom du fichier sans suffixe

# vim:ft=make
#
