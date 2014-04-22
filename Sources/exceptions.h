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

class Not16bitsGrayScalePicture: public exception {
private:
	int bitsPerSample,samplePerPixel;
	string file;
public:
    Not16bitsGrayScalePicture(int bitsPerSample,int samplePerPixel, string file) throw();
	virtual const char* what() const throw();
    virtual ~Not16bitsGrayScalePicture() throw();
};

class ErrorOpeningPicture: public exception {
private:
	string file;
public:
	ErrorOpeningPicture(string file) throw();
	virtual const char* what() const throw();
	virtual ~ErrorOpeningPicture() throw();
};

#endif /* defined(__climso_auto__exceptions__) */
