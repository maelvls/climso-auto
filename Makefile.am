# Makefile.am par Mael Valais - 2014
# Nécessaire à automake

CFLAGS=-g --pedantic -Wall -std=c99 -O2
CXXFLAGS=-g
bin_PROGRAMS=classe_image_test lk_test

classe_image_test_SOURCES=\
	src/main_image.cpp\
	src/cmd_arduino.c\
	src/convol.c src/crea_tiff_3.c src/fcts_LK3.c src/interpol.c\
	src/csbigcam.cpp src/csbigimg.cpp src/diametre_soleil.c\
  	src/exceptions.cpp  src/image.cpp
lk_test_SOURCES=\
	src/main_position.c\
	src/cmd_arduino.c\
	src/convol.c src/crea_tiff_3.c src/fcts_LK3.c src/interpol.c\
	src/diametre_soleil.c
#classe_image_test_LDFLAGS = 
classe_image_test_CPPFLAGS =-DDEBUG
