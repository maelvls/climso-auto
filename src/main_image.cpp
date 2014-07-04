//
//  main_image.cpp
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include "image.h"
#define SEUIL	0.70

int main(int argc, char **argv) {
	Image* lapl_obj = Image::depuisTiff("lapl_obj.tif");
	Image* lapl_ref = Image::depuisTiff("lapl_ref.tif");
	Image* correl = lapl_obj->correlation(*lapl_ref,SEUIL);
	correl->versTiff("correl.tif");
	correl = lapl_obj->correlation_rapide_centree(*lapl_ref,SEUIL);
	correl->versTiff("correl2.tif");
}

