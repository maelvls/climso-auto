//
//  exceptions.h
//  climso-auto
//
//  Created by Maël Valais on 22/04/2014.
//

#ifndef __climso_auto__exceptions__
#define __climso_auto__exceptions__

#include <iostream>
#include <sstream>
#include <exception>
using namespace std;

class FormatException: public exception {
public:
	int bitsPerSample,samplePerPixel;
	string file;
    FormatException(int bitsPerSample,int samplePerPixel, string file) throw();
	virtual const char* what() const throw();
    virtual ~FormatException() throw();
};

class OpeningException: public exception {
public:
	string file;
	OpeningException(string file) throw();
	virtual const char* what() const throw();
	virtual ~OpeningException() throw();
};

#endif /* defined(__climso_auto__exceptions__) */
