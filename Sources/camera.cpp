//
//  camera.c
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <stdio.h>
#include "csbigcam.h"
#include "csbigimg.h"

void transformerEnTableauDeDouble(CSBIGImg *img_src, double **img_dest, int *hauteur_dst, int *largeur_dst) {
	*hauteur_dst = img_src->GetHeight();
	*largeur_dst = img_src->GetWidth();
	
	img_dest = new double*[*hauteur_dst];
	for (int lign=0; lign< *hauteur_dst; lign++) {
		img_dest[lign] = new double[*largeur_dst];
		for (int col=0; col< *largeur_dst; col++) {
			img_dest[lign][col] = img_src->GetImagePointer()[lign * (*largeur_dst) + col];
		}
	}
}

