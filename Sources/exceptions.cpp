//
//  exceptions.cpp
//  climso-auto
//
//  Created by Maël Valais on 22/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//


#include "exceptions.h"

FormatException::FormatException(int bitsPerSample,int samplePerPixel, string file) throw() :bitsPerSample(bitsPerSample),samplePerPixel(samplePerPixel),file(file)
{}
	
const char* FormatException::what() const throw()
{
	//TODO: essayer de donner une vrai exception
	//ostringstream stream;
	//stream << "L'image" << file << " ne semble pas être 16 bits à niveaux de gris : "
	//	<< "il y a " << bitsPerSample << "bits par sample (au lieu de 16)"
	//	<< "et " << samplePerPixel << " samples par pixel (au lieu de 1) << endl";
	//return stream.str().c_str();
	return "L'image ne semble pas etre en 16bits niveaux de gris\n";
}

FormatException::~FormatException() throw() {
	//TODO: faut il delete des trucs ?
}



OpeningException::OpeningException(string file) throw() :file(file) {
}

const char* OpeningException::what() const throw()
{
	//TODO: essayer de donner une vrai exception
	//ostringstream stream;
	//stream << "L'image" << file << " n'a pas pu etre lue" << endl;
	//return stream.str().c_str() ;
	return "L'image n'a pas pu etre lue\n";
}

OpeningException::~OpeningException() throw() {
	//TODO: faut il delete des trucs ?
}