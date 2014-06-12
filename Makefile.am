# Makefile.am par Mael Valais - 2014
# Nécessaire à automake

# Ce fichier contient les noms des fichiers qui permettent la compilation Autotools

# Les fichiers Makefile.am (vides) dans tous les sous-dossiers sont obligatoires mais
# j'ai décidé de tout mettre dans celui-ci pour simplifier (grâce à subdir-objects dans 
# configure.ac)

# CONCERNANT Qt (à travers autotroll.m4 et autotroll.mk) : 
# - include autotroll.mk permet d'ajouter les règles implicites pour compiler 
#		les .ui en _ui.h, les .moc en .moc.cpp...
# - BUILT_SOURCES= contient tous les fichiers .ui ou .moc ou .rcc qui devront être
# 		compilés par les outils uic, moc ou rcc. Si on met mainwindow_ui.h dans
#		prog_SOURCES par exemple, il faudra qu'on écrive : 
#		BUILT_SOURCES=mainwindow_ui.h

# Appel au makefile pour les moc, uic, rcc 
# provenant de autotroll
include autotroll.mk

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
#test_classe_image_HEADERS=
#	src/mainwindow.ui
  	
test_classe_image_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
test_classe_image_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) -DDEBUG
test_classe_image_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS)
test_classe_image_LDADD    = $(QT_LIBS) $(LDADD)

# Fichiers qui devront être compilés par uic ou moc avant la compilation
# Voir CONCERNANT Qt au dessus
BUILT_SOURCES=
