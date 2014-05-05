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
    Image a,b;
	try {
		a = Image::chargerTiff("/Users/mael65/prog/images-de-correlation/test-convol/obj.tif");
		b = Image::chargerTiff("/Users/mael65/prog/images-de-correlation/test-convol/ref.tif");

	//	a = Image::chargerTiff("/Users/mael65/prog/images-de-correlation/test-convol-grand/lapl_obj.tif");
	//	b = Image::chargerTiff("/Users/mael65/prog/images-de-correlation/test-convol-grand/lapl_ref.tif");

	}
	catch(exception& e) { // On gère les différentes erreurs
		cerr << "Erreur : " << e.what();
	}
	
	//a.ecrireTiff("/Users/mael65/prog/images-de-correlation/test-convol/obj-out.tif");
	//b.ecrireTiff("/Users/mael65/prog/images-de-correlation/test-convol/ref-out.tif");

	a.afficher();
	b.afficher();
	Image c = a.convolution(b,0.85);

	c.ecrireTiff("/Users/mael65/prog/images-de-correlation/test-convol/convol.tif");
	//c.ecrireTiff("/Users/mael65/prog/images-de-correlation/test-convol-grand/convol.tif");

}
