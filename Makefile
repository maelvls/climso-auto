#
# makefile
# Mael Valais, 2014-04-07 15:23
#
# Makefile principal
#

OPT=-g
CC=clang
CPP=clang++
LIBS=-L /usr/local/lib
SOURCES=

BIN=a.out
# Attention, "all", "arduino"... ont besoin d'un main.c
MAIN_TEST_ALL=main_global.c
MAIN_TEST_ARDUINO=main_arduino.c
MAIN_TEST_CAMERA=main_camera.c
MAIN_TEST_POSITION=main_position.c


SRCDIR=Sources
OBJDIR=Builds

# On prépare la liste des .c/cpp puis la liste des .o
LIST_SOURCES_C=$(filter-out %main%, $(wildcard $(SRCDIR)/*.c)) # */ # On enlève les main.c
LIST_SOURCES_CPP=$(filter-out %main%, $(wildcard $(SRCDIR)/*.cpp)) # */ # On enlève les main.cpp
LIST_OBJ_1=$(LIST_SOURCES_C:.c=.o) # On ajoute les .c et on remplace par .o
LIST_OBJ_1+=$(LIST_SOURCES_CPP:.cpp=.o) # On ajoute les .cpp et on remplace par .o
LIST_OBJ_2=$(notdir $(LIST_OBJ_1))
LIST_OBJ=$(addprefix $(OBJDIR)/,$(LIST_OBJ_2)) # Tous les objets sont Builds/objet.o
# On a une liste de tous les .c/.cpp et une liste de .o

arduino: $(OBJDIR)/$(MAIN_TEST_ARDUINO:.c=.o) build

camera: $(OBJDIR)/$(MAIN_TEST_CAMERA:.c=.o) build

position: $(OBJDIR)/$(MAIN_TEST_POSITION:.c=.o) build

all: $(OBJDIR)/$(MAIN_TEST_ALL:.c=.o) build

build: $(LIST_OBJ)
	@echo $(LIST_OBJ)
	$(CPP) $^ -o $(OBJDIR)/$(BIN) $(OPT) $(LIBS)

#
# Build des Sources
#
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC)  -I. -o $@ -c $<
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) -I. -o $@ -c $<


clean:
	rm -rf $(OBJDIR)/*.o #*/



#$@	 Le nom de la cible
#$<	 Le nom de la première dépendance
#$^	 La liste des dépendances
#$?	 La liste des dépendances plus récentes que la cible
#$*	 Le nom du fichier sans suffixe

# vim:ft=make
#
