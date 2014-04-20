#
# makefile
# Mael Valais, 2014-04-07 15:23
#
# Makefile permettant la compilation du projet climso-auto
# NOTE: les # */ sont là à cause de Xcode
#
# A faire :
#	- comprendre VPATH (les espaces ? pas d'espaces?) (OK)
#	- essayer de comprendre les .d et -MF
#	- comprendre ce qu'est un CFLAGS, CXXFLAGS
#

# Déroulement d'une compil :
#		cc -E
#
#
#
#

#
# Variables diverses
#
# Dossier des .c, .cpp
SRCDIR=Sources
# Dossiers dans lesquels sont les libs qui seront compilées
SRCLIBDIR=Libraries/arduino_serial_lib #Libraries/camera_sbig_lib
# Dossier des objets .o
OBJDIR=Builds
# Dossier des exécutables .out
BINDIR=Binaries
# Nom de l'exécutable en sortie
BIN=a.out
# Librairies (exple -L/usr/local/lib) utilisées par le linker
LIBS=


#
# Les variables des règles implicites et explicites
#
CFLAGS=-g		# Les flags de compilation des .c
CXXFLAGS=-g	# Les flags de compilation des .cpp
CPPFLAGS=		# Les flags de pré-processeur (cc -E)
CC=clang		# Compilateur .c
CXX=clang++		# Compilateur .cpp
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

#
# Préparation du VPATH qui permettra à make de chercher les sources aux bons endroits
# lors de la phase de build
#
VPATH := $(SRCDIR) $(SRCLIBDIR) # */ # Où trouver les SOURCES (libs, .c...)


# Une fonction "FILTER-OUT" pour supprmier les éventuels main.c
FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
#
# Construction de la liste des objets à build des les sources
#
LIST_OBJ := $(subst .c,.o,$(wildcard $(SRCDIR)/*.c)) # */
LIST_OBJ += $(subst .cpp,.o,$(wildcard $(SRCDIR)/*.cpp)) # */
LIST_OBJ := $(call FILTER_OUT,main,$(LIST_OBJ)) # */ # On enlève les main
LIST_OBJ := $(addprefix $(OBJDIR)/,$(notdir $(LIST_OBJ))) # On enlève les repertoires
#
# Construction de la liste des objets à build des les librairies
#
LIST_OBJ_LIB := $(subst .c,.o,$(foreach v,$(SRCLIBDIR),$(wildcard $(v)/*.c))) # */
LIST_OBJ_LIB += $(subst .cpp,.o,$(foreach v,$(SRCLIBDIR),$(wildcard $(v)/*.cpp))) # */
LIST_OBJ_LIB := $(call FILTER_OUT,main,$(LIST_OBJ_LIB)) # */ # On enlève les main
LIST_OBJ_LIB := $(addprefix $(OBJDIR)/,$(notdir $(LIST_OBJ_LIB))) # On enlève les repertoires
#
# Construction des includes (les headers)
#
INCLUDES := $(SRCDIR) $(SRCLIBDIR)
INCLUDES := $(addprefix -I,$(INCLUDES))

#
# Build (sources, librairies)
#

$(OBJDIR)/%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ -c $<
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o $@ -c $<


arduino: $(OBJDIR)/$(MAIN_TEST_ARDUINO:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN)

camera: $(OBJDIR)/$(MAIN_TEST_CAMERA:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN)

position: $(OBJDIR)/$(MAIN_TEST_POSITION:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN)

all: $(OBJDIR)/$(MAIN_TEST_ALL:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN)

clean:
	$(RM)

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
