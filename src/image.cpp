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
    pic = new int* [heigth*width];
}



Image::Image() {
    for (int i=0; i<width; i++) {
        delete [] pic[i];
    }
    delete [] pic;
}

