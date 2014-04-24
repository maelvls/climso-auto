//
//  main_image.c
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <iostream>
#include "image.h"

using namespace std;

int main(int argc, const char *argv[])
{
    Image a;
	try {
		a = Image::chargerTiff("test16bits.tif");
	}
	catch(exception const& e) { // On gère les différentes erreurs
		cerr << "Erreur : " << e.what();
	}
	a.ecrireTiff("test-out.tif");
	a.~Image();
}
