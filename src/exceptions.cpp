/*
 *  exceptions.cpp
 *  climso-auto
 *
 *  Created by Maël Valais on 22/04/2014.
 *
 *	Cette classe apporte les exceptions possible pour la classe Image.
 *
 *	NOTE: Cette classe devrait être amenée à disparaitre car je n'ai pas
 *	fait le choix d'utiliser des exceptions. Ceci dit, leur utilité lorsqu'on
 *	lit une image (fichier inexistant...) permet de debugger même si
 *	l'utilisateur de la classe a oublié d'implémenter des garde-fous.
 *
 *	Exceptions à ajouter :
 * 	- si on ajoute une valeur trop grande
 * 	- si on est en dehors des indices de l'image
 */


#include "exceptions.h"

/**
 * Exception de format (nombre de samples par pixel ou nombre de bits par sample)
 * @param bitsPerSample
 * @param samplePerPixel
 * @param file
 */
FormatException::FormatException(int bitsPerSample,int samplePerPixel, string file) throw()
	:bitsPerSample(bitsPerSample),samplePerPixel(samplePerPixel),file(file) {}
	
const char* FormatException::what() const throw() {
	return "Le fichier demande n'a pas le bon nombre de samples par pixel ou bits par samples\n";
}

FormatException::~FormatException() throw() {}


/**
 * Exception d'ouverture du fichier
 * @param file
 */
OpeningException::OpeningException(string file) throw() :file(file) {}

const char* OpeningException::what() const throw() {
	return "L'image n'a pas pu etre lue\n";
}

OpeningException::~OpeningException() throw() {}
