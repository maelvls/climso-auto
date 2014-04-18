//
//  image.cpp
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include "image.h"

Image::Image(int hauteur, int largeur) {
    h = hauteur;
    l = largeur;
    img = new int* [h * l];
}

int Image::chargerTiff(string fichierEntree) {
    return 0;
}

Image::Image() {
    delete [] img;
}

