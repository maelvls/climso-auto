# Makefile.am par Mael Valais - 2014
# Nécessaire à automake

# Pour que aclocal puisse trouver les fichiers .m4
ACLOCAL_AMFLAGS = -I m4

CFLAGS=-g --pedantic -Wall -std=c99 -O2
CXXFLAGS=-g
bin_PROGRAMS=test_classe_image

test_classe_image_SOURCES=\
	src/main_image.cpp\
	src/cmd_arduino.c\
	src/interpol.c\
	src/csbigcam.cpp src/csbigimg.cpp src/diametre_soleil.c\
  	src/exceptions.cpp  src/image.cpp
#test_classe_image_LDFLAGS = 
test_classe_image_CPPFLAGS =-DDEBUG
