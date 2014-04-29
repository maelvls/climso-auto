//
//  exceptions.h
//  climso-auto
//
//  Created by Maël Valais on 22/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef __climso_auto__exceptions__
#define __climso_auto__exceptions__

#include <iostream>
#include <sstream>
using namespace std;

class FormatException: public exception {
private:
	int bitsPerSample,samplePerPixel;
	string file;
public:
    FormatException(int bitsPerSample,int samplePerPixel, string file) throw();
	virtual const char* what() const throw();
    string& toString() const throw();
    virtual ~FormatException() throw();
};

class OpeningException: public exception {
private:
	string file;
public:
	OpeningException(string file) throw();
	virtual const char* what() const throw();
	string& toString() const throw();
	virtual ~OpeningException() throw();
};

#endif /* defined(__climso_auto__exceptions__) */
