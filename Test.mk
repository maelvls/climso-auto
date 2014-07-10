
%.o: %.c
	gcc -o $@ -c $< -g
%.o: %.cpp
	g++ -o $@ -c $< -g

all: src/interpol.o src/convol.o src/image.o src/main_image.o src/exceptions.cpp
	g++ $^ -o a.out -ltiff -lm

clean:
	rm -f src/*.o
	rm -f a.out

# Les fichiers de dépendance .d (cc -E appelle le pré-processeur)
#		$(CC) -E -MM $^ -MF leFichierDesDependances.d
# Exemple d'appel :
#		foo.o: foo.c
#			cc -E -MM $^ -MF leFichierDesDependances.d ($^ == foo.c)
# donnera dans leFichierDesDependances.d :
#		foo.o: foo.c foo.h bar.c bar.h
#

### Memo des variables automatiques ###
#	$@	 Le nom de la cible
#	$<	 Le nom de la première dépendance
#	$^	 La liste des dépendances
#	$?	 La liste des dépendances plus récentes que la cible
#	$*	 Le nom du fichier sans suffixe

# vim:ft=make
#
