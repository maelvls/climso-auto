//
//  picture.h
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef __climso_auto__picture__
#define __climso_auto__picture__

#include <iostream>
using namespace::std;

typedef struct _pixel {
    short pix;
    int x,y;
} Pixel;

class Picture {
private:
    int heigth, width;
    short **pic;
public:
    Picture();
    Picture(int hauteur, int largeur);
    Picture(Picture&);
    Picture(Picture&,int decal_x, int decal_y, int longueur, int largeur);
    Picture(Picture&, int facteur_bin);
    
    int chargerTiff(string fichierEntree);
    int ecrireTiff(string fichierSortie);
    
    short pix(int x, int y); // Il faudrait faire un itérateur
    int hauteur();
    int largeur();
    
    Pixel& trouverMaximum();
    Picture& convolution(Picture& p);
    Picture& operator*(Picture&); // convolution
    Picture& correlation(Picture& p);
    
    ~Picture();
};

#endif /* defined(__climso_auto__picture__) */
