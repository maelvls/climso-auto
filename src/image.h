/*
 *  image.h
 *  climso-auto
 *
 *  Created by Maël Valais on 15/04/2014.
 *  Copyright (c) 2014 Maël Valais. All rights reserved.
 */

#ifndef __climso_auto__picture__
#define __climso_auto__picture__

// Quelles fonctions externes sont utilisée ?
#define INCLUDE_INTERPOL	1	// Fonctions contenues dans interpol.c (maxParInterpolation()...)
#define INCLUDE_CONVOL		0   // Fonction de calcul de correlation données par Laurent Koechlin
#define INCLUDE_TIFF		1 	// Si 1, alors -L/usr/local/lib et -ltiff
#define INCLUDE_SBIGCAM		1
#define INCLUDE_SBIGIMG		1 	// Si 1, alors	(MAC) -F/Library/Frameworks et -framework SBIGUDrv (selon installation)
								//				(LINUX) -L/usr/local/lib et -lsbigudrv (selon installation)

#include <iostream>
#include <sstream>
using namespace std;
#include "exceptions.h"

#if INCLUDE_TIFF
	#include <tiffio.h>
#endif
#if INCLUDE_SBIGIMG
    #include "csbigimg.h"
#endif
#if INCLUDE_SBIGCAM
	#include "csbigcam.h"
#endif
#if INCLUDE_INTERPOL
	#include "interpol.h"
#endif
#if INCLUDE_CONVOL
	#include "convol.h"
#endif

#define NOMBRE_BITS_PAR_SAMPLE		16 // Taille arbitraire (uint16_t)
#define NOMBRE_SAMPLES_PAR_PIXEL	1  // Car c'est des niveaux de gris

#define INTENSITE_MAX               65535   // Lors d'éventuelles normalisations

const int NOYAU_LAPLACIEN_TAB[] = {0,-1,0,-1,4,-1,0,-1,0};
const int NOYAU_LAPLACIEN_TAILLE = 3;

typedef double MonDouble;

class Image {
private:
    int lignes, colonnes; // hauteur, largeur
    MonDouble *img; // Des pixels nuances de gris sur 16 bits
    int max_c, max_l;
    int min_c, min_l;
    bool determinerMinMax();
public:
    Image();
    Image(int hauteur, int largeur);
    Image(Image& src);
    Image(Image& src, int ligne_0, int col_0, int hauteur, int largeur);
    ~Image();

    // Opérations sur les images
    void copier(Image& src, int ligne_0, int col_0, int hauteur, int largeur);
    void copier(Image& src);
    void init(int val);
    void normaliser();
    void normaliser(MonDouble minSortie, MonDouble maxSortie);
    Image* reduire(int facteur_binning);
    MonDouble valeurMin();
    MonDouble valeurMax();
    int posMinLigne();
    int posMinColonne();
    int posMaxLigne();
    int posMaxColonne();
    void tracerDonut(int l_centre, int c_centre, double freq_min, double marge_int, double freq_max, double marge_ext);
    static Image* tracerFormeSoleil(int diametre);

    // Correlations ecrites par Mael Valais (pour apprendre..)
    Image* correlation_simple(Image& p, float seuil_ref);
    Image* correlation_rapide(Image& reference, float seuil_ref); // Correlation optimisée
    Image* correlation_rapide_centree(Image& reference, float seuil_ref); // Correlation optimisée
    Image* convoluer(const int *noyau, int taille);
    Image* convoluerParDerivee();
    Image* deriveeCarre();
    double calculerSignalSurBruit(int l, int c);

    // Entrées/sorties (depuis/vers)
    unsigned char* versUchar();
#if INCLUDE_TIFF
    static Image* depuisTiff(string fichierEntree);
    void versTiff(string fichierSortie);
#endif

#if INCLUDE_SBIGIMG
    static Image* depuisSBIGImg(CSBIGImg &img);
#endif
    static Image* depuisTableauDouble(double ** tableau, int hauteur, int largeur);
    double** versTableauDeDouble();

    // Getters/Setters
    int getLignes()  {return lignes; }
    int getColonnes() { return colonnes;}
    MonDouble* ptr() { return img; }
    MonDouble getPix(int l, int c) { return img[l*colonnes + c];}
    void setPix(int l, int c, MonDouble intensite) { img[l*colonnes + c]=intensite;}

    void afficher(); // Affiche les intensités brutes, pour debug
	
	// --------- Fonctions liées à interpol.h  ------
#if INCLUDE_INTERPOL
	Image* interpolerAutourDeCePoint(int l, int c);
	Image* interpolerAutourDeCePoint(int l, int c, float pas_interp, float marge);
	void maxParInterpolation(double *l, double *c);
#endif

};

#endif /* defined(__climso_auto__picture__) */
