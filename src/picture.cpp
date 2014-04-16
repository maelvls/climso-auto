//
//  picture.cpp
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include "picture.h"

Picture::Picture(int hauteur, int largeur) {
    heigth = hauteur;
    width = largeur;
    pic = new short* [heigth];
    for (int i=0; i<width; i++) {
        pic[i] = new short [width];
    }
}

Picture::~Picture() {
    for (int i=0; i<width; i++) {
        delete [] pic[i];
    }
    delete [] pic;
}

