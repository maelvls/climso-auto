#
# makefile
# Mael Valais, 2014-04-07 15:23
#
# Makefile principal
#

OPT=-g -lsbigudrv
CC=clang
CPP=clang++
INCLUDES=-I Sources/arduino_library Sources/camera_sbig_library
LIBS=-L /usr/local/lib

BIN=a.out

SRCDIR=Sources
OBJDIR=Builds
BINDIR=.
LE_MAIN=main.c

LIST_SOURCES_C=$(filter-out %main.c, $(wildcard $(SRCDIR)/*.c)) # */ # On enlève les main.c
LIST_SOURCES_CPP=$(filter-out %main.cpp, $(wildcard $(SRCDIR)/*.cpp)) # */ # On enlève les main.cpp
LIST_OBJ_1=$(LIST_SOURCES_C:.c=.o) # On ajoute les .c et on remplace par .o
LIST_OBJ_1+=$(LIST_SOURCES_CPP:.cpp=.o) # On ajoute les .cpp et on remplace par .o
LIST_OBJ_1+=$(LE_MAIN:.c=.o) # On ajoute LE MAIN
LIST_OBJ_2=$(notdir $(LIST_OBJ_1))
LIST_OBJ=$(addprefix $(OBJDIR)/,$(LIST_OBJ_2)) # Tous les objets sont Builds/objet.o

LIST_SRC_LIB_C=$(filter-out %main.c, $(wildcard $(SRCDIR)/*/*.c)) # */ # On enlève les main.c
LIST_SRC_LIB_CPP=$(filter-out %main.cpp, $(wildcard $(SRCDIR)/*/*.cpp)) # */ # On enlève les main.cpp
LIST_OBJ_LIB_0=$(LIST_SRC_LIB_C:.c=.o) # On ajoute les .c et on remplace par .o
LIST_OBJ_LIB_0+=$(LIST_SRC_LIB_CPP:.cpp=.o) # On ajoute les .cpp et on remplace par .o
LIST_OBJ_LIB_1=$(notdir $(LIST_OBJ_LIB_0))
LIST_OBJ_LIB=$(addprefix $(OBJDIR)/,$(LIST_OBJ_LIB_1)) # Tous les objets sont Builds/objet.o

# On a une liste de tous les .c et .cpp et une liste de .o

all: $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CPP) $^ -o $(BIN) $(OPT) $(LIBS)

#
# Build des Sources
#

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(INCLUDES) -o $@ -c $<
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INCLUDES) -o $@ -c $<

#
# Build des librairies
#



# arduino_library
$(OBJDIR)/%.o: $(SRCDIR)/arduino_library/%.c
	$(CC) -o $@ -c $<
$(OBJDIR)/%.o: $(SRCDIR)/arduino_library/%.cpp
	$(CPP) -o $@ -c $<
# camera_sbig_library
# Attention, on ajoute -lsbigudrv
$(OBJDIR)/%.o: $(SRCDIR)/camera_sbig_library/%.c
	$(CC) -o $@ -c $<
$(OBJDIR)/%.o: $(SRCDIR)/camera_sbig_library/%.cpp
	$(CPP) -o $@ -c $<
#
# Eventuels fichiers dans Sources
#
$(OBJDIR)/%.o: %.c
	$(CC) $(INCLUDES) -o $@ -c $<

%: $(filter %.c %.cpp, wildcard($(SRCDIR)/$@/*)) # */ # On prend les
	@echo "lol"


	
	
clean:
	rm -rf $(BINDIR)/$(BIN) $(OBJDIR)/*.o #*/











#$@	 Le nom de la cible
#$<	 Le nom de la première dépendance
#$^	 La liste des dépendances
#$?	 La liste des dépendances plus récentes que la cible
#$*	 Le nom du fichier sans suffixe

# vim:ft=make
#
