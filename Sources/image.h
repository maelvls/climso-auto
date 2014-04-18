//
//  image.h
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef __climso_auto__picture__
#define __climso_auto__picture__

#include <iostream>
using namespace::std;

typedef struct {
    short pix;
    int x,y;
} Pixel;

class Image {
private:
    int h, l; // hauteur, largeur
    int **img;
public:
    Image();
    Image(int hauteur, int largeur);
    Image(Image&);
    Image(Image&,int decal_x, int decal_y, int longueur, int largeur);
    Image(Image&, int facteur_bin);
    
    int chargerTiff(string fichierEntree);
    int ecrireTiff(string fichierSortie);
    
    short pix(int x, int y); // Il faudrait faire un itérateur
    int hauteur();
    int largeur();
    
    Pixel& trouverMaximum();
    Image& convolution(Image& p);
    Image& operator*(Image&); // convolution
    Image& correlation(Image& p);
    
    ~Image();
};

#endif /* defined(__climso_auto__picture__) */
