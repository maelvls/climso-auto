//
//  image.h
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef __climso_auto__picture__
#define __climso_auto__picture__

#include <exception>
#include <iostream>
#include <sstream>
#include <tiffio.h>

#define NOMBRE_BITS_PAR_SAMPLE		16 // Taille arbitraire (uint16_t)
#define NOMBRE_SAMPLES_PAR_PIXEL	1  // Car c'est des niveaux de gris


#include "exceptions.h"
using namespace::std;

typedef struct {
    short pix;
    int x,y;
} Pixel;

class Image {
private:
    int lignes, colonnes; // hauteur, largeur
    double **img; // Des pixels nuances de gris sur 16 bits
public:
    Image();
    Image(int hauteur, int largeur);
    //Image(Image&);
    //Image(Image&,int decal_x, int decal_y, int longueur, int largeur);
    //Image(Image&, int facteur_bin);
    
    static Image& chargerTiff(string fichierEntree);
    int ecrireTiff(string fichierSortie);
    
    void copier(Image& src, int x, int y);
    void copier(Image& src);
    void init(int val);

    double getPix(int l, int c); // Il faudrait faire un itérateur
    void setPix(int l, int c, double intensite);
    int getLignes();
    int getColonnes();
    
    Pixel& trouverMaximum();
    Image& convolution(Image& p, double seuil);
    Image& convolution(Image& p);
    Image& operator*(Image&); // convolution
    Image& correlation(Image& p);
	
    ~Image();
};


#endif /* defined(__climso_auto__picture__) */
