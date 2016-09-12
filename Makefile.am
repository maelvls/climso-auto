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
bin_PROGRAMS=climsoauto

# Fichiers source (.c, .cpp, .h ou tout autre fichier utile aux étapes de compilation)
climsoauto_SOURCES=\
	src/arduino.cpp\
	src/interpol.c\
	src/csbigcam.cpp src/csbigimg.cpp\
	src/exceptions.cpp  src/image.cpp\
	src/fenetreprincipale.cpp src/fenetreprincipale.moc.cpp\
	src/main_gui.cpp\
	src/guidage.cpp src/guidage.moc.cpp\
	src/widgetimage.cpp src/widgetimage.moc.cpp \
	src/capture.cpp src/capture.moc.cpp src/diametre_soleil.c\
	src/parametres.cpp src/parametres.moc.cpp\
	src/camera_sbig.cpp src/camera_fake.cpp\
	\
	src/arduino.h\
	src/interpol.h\
	src/exceptions.h src/image.h\
	src/fenetreprincipale.h src/fenetreprincipale_ui.h\
	src/csbigcam.h src/csbigimg.h\
	src/lpardrv.h\
	src/guidage.h\
	src/widgetimage.h\
	src/capture.h src/diametre_soleil.h\
	src/parametres.h\
	src/camera_sbig.h src/camera_fake.h\
	\
	src/fenetreprincipale.ui

# -Ox correspond au mode d'optimisation. -O2 est une bonne optimisation.
# -O0 désactive l'optimisation  	
CFLAGS=-g --pedantic -Wall -std=c99 -O2
CXXFLAGS=-g -O0
climsoauto_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
climsoauto_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS)
climsoauto_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS)
climsoauto_LDADD    = $(QT_LIBS) $(LDADD)

# Fichiers qui devront être compilés par uic ou moc avant la compilation
# Voir CONCERNANT Qt au dessus
BUILT_SOURCES=\
	src/fenetreprincipale.moc.cpp src/fenetreprincipale_ui.h src/parametres_ui.h\
	src/guidage.moc.cpp src/widgetimage.moc.cpp src/parametres.moc.cpp\
	src/capture.moc.cpp

# Règle perso car la règle présente dans autotroll.mk (%ui_.h: %.ui) ne marche pas
# pour produire du src/ui_blabla.h (car il faut deux wildcards, genre %ui_%.h)
#src/ui_%.h: src/%.ui
#	$(UIC) $< -o $@

# NOTE sur $(srcdir) et $(includedir)
# il faudrait théoriquement préfixer tous les fichiers source par $(srcdir)
# et tous les fichiers header par $(includedir)
