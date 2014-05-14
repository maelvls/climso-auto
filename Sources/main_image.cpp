//
//  main_image.c
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <iostream>
#include <iomanip> // Pour cout << setprecision(4)
#include "image.h"
#include "csbigcam.h"
#include "csbigimg.h"

using namespace std;


int main(int argc, char **argv) {
    string emplacement = "/Users/mael65/prog/images-de-correlation/test-correlation-mv/";

	Image *ref = Image::dessinerMasqueDeSoleil(204);
    ref->versTiff(emplacement+"ref.tif");
	
    Image *obj = Image::depuisTiff(emplacement+"obj.tif");
  
    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
    Image *ref_lapl = ref->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);

    obj_lapl->versTiff(emplacement+"obj_lapl.tif");
    ref_lapl->versTiff(emplacement+"ref_lapl.tif");
	
    Image *correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
    correl->versTiff(emplacement+"correl.tif");
	
	double l,c;
//	correl->maxParInterpolation(&l, &c);
	
	const int taille = 20; // carré de 20 de pixels ; le max est au centre
	const float pas_interp = 1/8.0; // le pas d'interpolation
	
	int l_max, c_max;
	correl->maxPixel(&l_max, &c_max);
	
	Image *interp = correl->interpolerAutourDeCePoint(l_max, c_max, pas_interp, taille);
	
	interp->normaliser();
	interp->versTiff(emplacement+"interp.tif");
	
	// On retrouve maintenant la position sub-pixel
	int l_max_interp, c_max_interp;
	interp->maxPixel(&l_max_interp, &c_max_interp);
	l = l_max - taille/2 + l_max_interp * pas_interp;
	c = c_max - taille/2 + c_max_interp * pas_interp;
	
	
	cout << fixed << showpoint << setprecision(2);
	cout << "(l="<< l <<",c="<< c << ")" << endl;


    delete obj;
    delete ref;
    delete obj_lapl;
    delete ref_lapl;
    delete correl;
	return 0;
}

