#
# makefile
# Mael Valais, 2014-04-07 15:23
#
# Makefile permettant la compilation du projet climso-auto
# NOTE: les # */ sont là à cause de Xcode
#
# A faire :
#	- comprendre VPATH (les espaces ? pas d'espaces?) (OK)
#	- essayer de comprendre les .d et -MF (PRESQUE OK)
#	- comprendre ce qu'est un CFLAGS, CXXFLAGS (OK -> -g)
#

# Déroulement d'une compilation :
#	- pré-compilation puis compilation en .o grâce à CC (-I pour les includes)
#	- linkage des .o grâce à LD (-L pour les répertoires des librairies, -l pour les archives .a)
#		-lm cherche donc libm.a ou libm.dylib
#		-L/usr/local/lib permet à LD de trouver des trucs qui ne sont pas dans le PATH_LIB_JECONNAISPAS
#		par défaut

#
# Variables diverses
#
# Dossier des Sources (.c, .cpp,.h)
SRCDIR=Sources
# Dossier des librairies (.c, .cpp, .h)
SRCLIBDIR=Libraries

FILTER=main camera_sbig_lib

# Dossier des objets .o
OBJDIR=Builds
# Dossier des exécutables .out
BINDIR=Binaries
# Nom de l'exécutable en sortie
BIN=a.out

# Librairies EXTERNES (exple -L/usr/local/lib) utilisées par le linker
# Par défaut, c'est dans LD_LIBRARY_PATH (mais pas sur MacOSX je crois)
LIBS=-L/usr/local/lib -L/opt/local/lib
LIBS_INCLUDES=-I/usr/local/include -I/opt/local/include

#
# Les variables des règles implicites et explicites
#
CFLAGS=		# Les flags de compilation des .c
CXXFLAGS=	# Les flags de compilation des .cpp
CPPFLAGS=-g		# Les flags de pré-processeur (cc -E...)
CC=clang++		# Compilateur .c
CXX=clang++		# Compilateur .cpp
LDFLAGS=-lm -ltiff
RM=rm -rf $(OBJDIR)/* # */

# EXPLICATION :
# CC et CFLAGS sont des variables qui conditionnent les règles implicites ;
# Par exemple, si une dépendance foo.o ne trouve aucune règle exlicite
# "foo.o: foo.c", alors make fait appel à la règle implicite
#			foo.o: foo.c
#				$(CC) -c (la compilation)


#
# Définition des différents main.c liés à chaque règle (arduino, all...)
# NOTE: "all", "arduino"... ont besoin d'un main.c
#
MAIN_TEST_ALL=main_global.c
MAIN_TEST_ARDUINO=main_arduino.c
MAIN_TEST_CAMERA=main_camera.c
MAIN_TEST_POSITION=main_position.c
MAIN_TEST_IMAGE=main_image.c

#
# Préparation du VPATH qui permettra à make de chercher les sources aux bons endroits
# lors de la phase de build
#
VPATH := $(SRCDIR) $(SRCLIBDIR) # */ # Où trouver les SOURCES (libs, .c...)

#
# Fonctions
#


# Supprimer les blancs
# $(call nospaces,a b c  d ) donne abcd
space:=$(subst , ,)
nospaces=$(subst $(space),,$(1))
# NOTE : Wildcard permet de "développer" le contenu avec * par exemple (comme en shell)
# Wildcard récursif. Appel : $(call rwildcard, , *.c) pour le rep. courant
rwildcard=$(foreach d,$(wildcard $(1)*),$(call rwildcard,$d/,$(2)) $(filter $(subst *,%,$(2)),$d))
# Supprime tous les termes de CHAINE contenant TERME quelque part
# $(call filter_out_multiple,TERME,CHAINE)
filter_out = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
# Supprime tous les termes de CHAINE où existent le TERME1 ou TERME2...
# $(call filter_out_multiple,TERME1 TERME2 (...),CHAINE)
filter_out_multiple = $(foreach v,$(2), $(if $(call nospaces,$(foreach p,$(1),$(if $(findstring $(p),$(v)),n,))),,$(v)))

#
# Construction de la liste des objets à build des les sources
#
LIST_OBJ := $(subst .c,.o,$(call rwildcard,$(SRCDIR),*.c))
LIST_OBJ += $(subst .cpp,.o,$(call rwildcard,$(SRCDIR),*.cpp))
LIST_OBJ := $(call filter_out_multiple,$(FILTER),$(LIST_OBJ)) # On filtre (main.c..)
LIST_OBJ := $(addprefix $(OBJDIR)/,$(notdir $(LIST_OBJ))) # On enlève les repertoires
#
# Construction de la liste des objets à build des les librairies
#
LIST_OBJ_LIB := $(subst .c,.o,$(call rwildcard,$(SRCLIBDIR),*.c))
LIST_OBJ_LIB += $(subst .cpp,.o,$(call rwildcard,$(SRCLIBDIR),*.cpp))
LIST_OBJ_LIB := $(call filter_out_multiple,$(FILTER),$(LIST_OBJ_LIB)) # On filtre (main.c..)
LIST_OBJ_LIB := $(addprefix $(OBJDIR)/,$(notdir $(LIST_OBJ_LIB))) # On enlève les repertoires
#
# Construction des includes (les headers)
#
#USER_INCLUDES := $(SRCDIR) $(SRCLIBDIR)
#USER_INCLUDES := $(addprefix -I,$(USER_INCLUDES)) $(LIBS_INCLUDES)
USER_INCLUDES := $(addprefix -I,$(dir $(call rwildcard, ,*.h)))
#
# Build (sources, librairies)
#

$(OBJDIR)/%.o: %.c
	$(CC) $(USER_INCLUDES) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(USER_INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<


arduino: $(OBJDIR)/$(MAIN_TEST_ARDUINO:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $(LIBS) $(LDFLAGS) $^ -o $(BINDIR)/$(BIN)

camera: $(OBJDIR)/$(MAIN_TEST_CAMERA:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $(LIBS) $(LDFLAGS) $^ -o $(BINDIR)/$(BIN)

position: $(OBJDIR)/$(MAIN_TEST_POSITION:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $(LIBS) $(LDFLAGS) $^ -o $(BINDIR)/$(BIN)

image: $(OBJDIR)/$(MAIN_TEST_IMAGE:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $(LIBS) $(LDFLAGS) $^ -o $(BINDIR)/$(BIN)

all: $(OBJDIR)/$(MAIN_TEST_ALL:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $(LIBS) $(LDFLAGS) $^ -o $(BINDIR)/$(BIN)

clean:
	$(RM)


essai:
	@echo "---------------ESSAI----------------"
	@echo "Liste des objets : $(LIST_OBJ)"
	@echo "Liste des objets librairie : $(LIST_OBJ_LIB)"
	@echo "Essai filter_out_multiple : $(call filter_out_multiple,a b d,dd ee ab a)"

# On peut utiliser VPATH pour indiquer les chemins des dépendances :
# VPATH=$(SRCDIR):$(LIBDIR)...
# Ou utiliser vpath

# ATTENTION : apparement MAKE fait un cc -c -o cmd_arduino.o Sources/cmd_arduino.c
# tout seul de son côté à cause du VPATH ?

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
