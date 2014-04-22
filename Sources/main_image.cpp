//
//  main_image.c
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <iostream>
#include "image.h"

using namespace::std;

int main(int argc, const char *argv[])
{
    Image a;
	try {
		a = Image::chargerTiff("/Users/mael65/prog/climso-auto/test.tif");
	}
	catch(exception const& e) { // On gère les différentes erreurs
		cerr << "Erreur : " << e.what();
	}
	a.ecrireTiff("/Users/mael65/prog/climso-auto/test2.tif");
	a.~Image();
}