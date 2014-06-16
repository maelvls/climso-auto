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

# Nom des executables à compiler
bin_PROGRAMS=main_qt

# Fichiers source (.c, .cpp, .h ou tout autre fichier utile aux étapes de compilation)
main_qt_SOURCES=\
	src/arduino.cpp\
	src/interpol.c\
	src/csbigcam.cpp src/csbigimg.cpp src/diametre_soleil.c\
	src/exceptions.cpp  src/image.cpp\
	src/fenetreprincipale.cpp src/fenetreprincipale.moc.cpp\
	src/main_qt_gui.cpp\
	src/guidage.cpp src/guidage.moc.cpp\
	\
	src/arduino.h\
	src/interpol.h\
	src/diametre_soleil.h\
	src/exceptions.h src/image.h\
	src/fenetreprincipale.h src/fenetreprincipale_ui.h\
	src/csbigcam.h src/csbigimg.h\
	src/sbigudrv.h src/lpardrv.h\
	src/guidage.h
  	
CFLAGS=-g --pedantic -Wall -std=c99 -O2
CXXFLAGS=-g
main_qt_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
main_qt_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) -DDEBUG
main_qt_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS)
main_qt_LDADD    = $(QT_LIBS) $(LDADD)

# Fichiers qui devront être compilés par uic ou moc avant la compilation
# Voir CONCERNANT Qt au dessus
BUILT_SOURCES=src/fenetreprincipale.moc.cpp src/fenetreprincipale_ui.h src/guidage.moc.cpp


# NOTE sur $(srcdir) et $(includedir)
# il faudrait théoriquement préfixer tous les fichiers source par $(srcdir)
# et tous les fichiers header par $(includedir)
