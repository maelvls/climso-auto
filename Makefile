#
# makefile
# Mael Valais, 2014-04-07 15:23
#
# Makefile principal
# NOTE: les # */ sont là à cause de Xcode
#
# A faire :
#	- comprendre VPATH (les espaces ? pas d'espaces?) (OK)
#	- essayer de comprendre les .d et -MF
#	- comprendre ce qu'est un CFLAGS, CXXFLAGS
#

OPT=-g
CC=clang
CXX=clang++
LIBS= #-L /usr/local/lib

# CC et CFLAGS sont des variables qui conditionnent les règles implicites ;
# Par exemple, si une dépendance foo.o ne trouve aucune règle exlicite
# "foo.o: foo.c", alors make fait appel à la règle implicite
#			foo.o: foo.c
#				$(CC) -c (la compilation)


SRCDIR=Sources
SRC_LIBRARIES=Libraries/arduino_serial_lib #Libraries/camera_sbig_lib
# dossiers dans lesquelles sont les libs qui seront compilées

OBJDIR=Builds

BINDIR=Binaries
BIN=a.out

# Attention, "all", "arduino"... ont besoin d'un main.c
MAIN_TEST_ALL=main_global.c
MAIN_TEST_ARDUINO=main_arduino.c
MAIN_TEST_CAMERA=main_camera.c
MAIN_TEST_POSITION=main_position.c

# Petit truc pour transformer un " " en ":" (pour SRC_LIBRARIES)
espace := $(subst ,, ) # On prend un espace pour pouvoir substituer avec $(subst)

#
# Préparation du VPATH qui permettra à make de chercher les sources aux bons endroits
# lors de la phase de build
#
VPATH := $(SRCDIR) $(subst $espace,:,$(SRC_LIBRARIES)) # */ # Où trouver les SOURCES (libs, .c...)


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
LIST_OBJ_LIB := $(subst .c,.o,$(foreach v,$(SRC_LIBRARIES),$(wildcard $(v)/*.c))) # */
LIST_OBJ_LIB += $(subst .cpp,.o,$(foreach v,$(SRC_LIBRARIES),$(wildcard $(v)/*.cpp))) # */
LIST_OBJ_LIB := $(call FILTER_OUT,main,$(LIST_OBJ_LIB)) # */ # On enlève les main
LIST_OBJ_LIB := $(addprefix $(OBJDIR)/,$(notdir $(LIST_OBJ_LIB))) # On enlève les repertoires
#
# Construction des includes (les headers)
#
INCLUDES := $(SRCDIR) $(SRC_LIBRARIES)
INCLUDES := $(addprefix -I,$(INCLUDES))

#
# Build (sources, librairies)
#
$(OBJDIR)/%.o: %.c
#$(CC)  # Construction des dépendances
	$(CC) $(INCLUDES) -o $@ -c $<
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(INCLUDES) -o $@ -c $<

# ATTENTION : apparement MAKE fait un cc -c -o cmd_arduino.o Sources/cmd_arduino.c
# tout seul de son côté à cause du VPATH ?

arduino: $(OBJDIR)/$(MAIN_TEST_ARDUINO:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	@echo "----------> Liste des objets librairie : $(LIST_OBJ_LIB) -----"
	@echo "----------> Vpath : $(VPATH) ----"
	@echo "----------> Includes : $(INCLUDES) ----"
	$(CXX) $^ -o $(BINDIR)/$(BIN) $(OPT) $(LIBS)

camera: $(OBJDIR)/$(MAIN_TEST_CAMERA:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN) $(OPT) $(LIBS)

position: $(OBJDIR)/$(MAIN_TEST_POSITION:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN) $(OPT) $(LIBS)

all: $(OBJDIR)/$(MAIN_TEST_ALL:.c=.o) $(LIST_OBJ) $(LIST_OBJ_LIB)
	$(CXX) $^ -o $(BINDIR)/$(BIN) $(OPT) $(LIBS)

clean:
	rm -rf $(OBJDIR)/*.o $(BIN) #*/


# On peut utiliser VPATH pour indiquer les chemins des dépendances :
# VPATH=$(SRCDIR):$(LIBDIR)...
# Ou utiliser vpath

#$@	 Le nom de la cible
#$<	 Le nom de la première dépendance
#$^	 La liste des dépendances
#$?	 La liste des dépendances plus récentes que la cible
#$*	 Le nom du fichier sans suffixe

# vim:ft=make
#
