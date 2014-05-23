//
//  exceptions.cpp
//  climso-auto
//
//  Created by Maël Valais on 22/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//


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
