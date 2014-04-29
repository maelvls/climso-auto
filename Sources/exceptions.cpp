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
FormatException::FormatException(int bitsPerSample,int samplePerPixel, string file) throw() :bitsPerSample(bitsPerSample),samplePerPixel(samplePerPixel),file(file)
{}
	
const char* FormatException::what() const throw() {
	char* a = new char[100];
	sprintf(a,"Le fichier %s n'a pas le bon nombre de samples par pixel ou bits par samples\n"
			"Il y a %d bits par sample et %d samples par pixel\n",file.c_str(),bitsPerSample,samplePerPixel);
	return a;
}
string& FormatException::toString() const throw() {
	ostringstream stream;
	stream << file << " ne semble pas être 8/16 bits à niveaux de gris" << endl;
	stream << "Il y a " << bitsPerSample << " bits par sample (au lieu de 8/16)" << endl;
	stream << "Et " << samplePerPixel << " samples par pixel (au lieu de 1)" << endl;
	string *s = new string(stream.str());
	return *s;
	//return "L'image ne semble pas etre en 16bits niveaux de gris\n";
}

FormatException::~FormatException() throw() {
	//TODO: faut il delete des trucs ?
}


/**
 * Exception d'ouverture du fichier
 * @param file
 */
OpeningException::OpeningException(string file) throw() :file(file) {
}

const char* OpeningException::what() const throw() {
	return "L'image n'a pas pu etre lue\n";
}
string& OpeningException::toString() const throw()
{
	ostringstream stream;
	stream << "L'image" << file << " n'a pas pu etre lue" << endl;
	string *s = new string(stream.str());
	return *s;
}

OpeningException::~OpeningException() throw() {
	//TODO: faut il delete des trucs ?
}
