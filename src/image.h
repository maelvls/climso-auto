//
//  image.h
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#define VERSION_LINEAIRE	0 /// Version linéaire "simple pointeur" ("double pointeur" sinon) (voir NOTE1)


#define INCLUDE_TIFF		1 	// Si 1, alors -L/usr/local/lib et -ltiff
#define INCLUDE_SBIGCAM		1
#define INCLUDE_SBIGIMG		1 	// Si 1, alors	(MAC) -F/Library/Frameworks et -framework SBIGUDrv (selon installation)
								//				(LINUX) -L/usr/local/lib et -lsbigudrv (selon installation)
#if not VERSION_LINEAIRE
#define INCLUDE_CONVOL		1
#define INCLUDE_FCTS_LK3	1
#define INCLUDE_INTERPOL	1
#endif


#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef __climso_auto__picture__
#define __climso_auto__picture__


#include <exception>
#include <iostream>
#include <sstream>
using namespace std;

#include "exceptions.h"
/*
 * Exceptions à ajouter :
 * - si on ajoute une valeur trop grande
 * - si on est en dehors des indices de l'image
 */

#if INCLUDE_TIFF
	#include <tiffio.h>
#endif
#if INCLUDE_SBIGIMG
    #include "csbigimg.h"
#endif
#if INCLUDE_SBIGCAM
	#include "csbigcam.h"
#endif
#if INCLUDE_CONVOL
	#include "convol.h"
#endif
#if INCLUDE_FCTS_LK3
	#include "fcts_LK3.h"
#endif
#if INCLUDE_INTERPOL
	#include "interpol.h"
#endif

#define NOMBRE_BITS_PAR_SAMPLE		16 // Taille arbitraire (uint16_t)
#define NOMBRE_SAMPLES_PAR_PIXEL	1  // Car c'est des niveaux de gris

#define INTENSITE_MAX               65535   // Lors d'éventuelles normalisations

const int NOYAU_LAPLACIEN_TAB[] = {0,-1,0,-1,4,-1,0,-1,0};
const int NOYAU_LAPLACIEN_TAILLE = 3;

typedef double MonDouble;

/**
 * Une image équivaut à un tableau 2D de "double" (pour rester compatible avec les fonctions existantes)
 * J'ai choisit la convention "matrice" : un point est désigné par point(ligne,colonne)
 * en partant du point extrème nord-ouest. D'autres représentations conseillent point(x,y)
 * mais je n'ai pas pris cette convention.
 *
 * NOTE1: Concernant la représentation en mémoire de l'image, j'hésite encore beaucoup entre la représentation
 * linéaire (un seul tableau de "double" avec les lignes les unes à la suite des autres) et la représentation
 * matricielle, c'est à dire un tableau de tableaux de "double".
 * 			-> VOIR le #define VERSION_LINEAIRE
 *
 * NOTE2: Cette classe a un gros soucis avec la création multiple d'objets lourds : à chaque fois qu'on traite
 * une image, on crée un nouvel objet en mémoire. Dans une boucle, cela ralenti le processus...
 */

class Image {
private:
    int lignes, colonnes; // hauteur, largeur

#if VERSION_LINEAIRE
    MonDouble *img; // Des pixels nuances de gris sur 16 bits
#else
    MonDouble **img;
#endif
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
    Image* reduire(int facteur_binning);
    void minMaxPixel(int *l_min, int *c_min, int *l_max, int *c_max);
    void maxPixel(int *l, int *c);
    Image* correlation_MV(Image& p, float seuil_ref);
    Image* correlation_reduite_MV(Image& reference, float seuil_ref);
#if VERSION_LINEAIRE
    Image* correlation_reduite2_MV(Image& reference, float seuil_ref);
#endif
    Image* convoluer(const int *noyau, int taille);
    Image* deriveeCarre();
    double sommePixels();

    // Entrées/sorties (depuis/vers)
#if INCLUDE_TIFF
    static Image* depuisTiff(string fichierEntree);
    void versTiff(string fichierSortie);
#endif

#if INCLUDE_SBIGIMG
    static Image* depuisSBIGImg(CSBIGImg &img);
#endif
    static Image* depuisTableauDouble(double ** tableau, int hauteur, int largeur);
    void versTableauDeDouble(double **tab, int *hauteur_dst, int *largeur_dst);

    // Getters/Setters
    int getLignes()  {return lignes; }
    int getColonnes() { return colonnes;}

#if VERSION_LINEAIRE
    MonDouble* ptr() { return img; }
    MonDouble getPix(int l, int c) { return img[l*colonnes + c];}
    void setPix(int l, int c, MonDouble intensite) { img[l*colonnes + c]=intensite;}
#else
    MonDouble** ptr() { return img; }
	MonDouble getPix(int l, int c) { return img[l][c];}
	void setPix(int l, int c, MonDouble intensite) {img[l][c]=intensite;}
#endif

/*
	// Pour detecter d'eventuels acces interdits lors de debugs
	MonDouble getPix(int l, int c) {
		if(l < 0 || l >= lignes || c < 0 || c >= colonnes)
			throw "getPix: Pixel hors-image";
		return img[l][c];
	}

	void setPix(int l, int c, MonDouble intensite) {
		if(l < 0 || l >= lignes || c < 0 || c >= colonnes)
			throw "setPix: Pixel hors-image";
		img[l][c]=intensite;
	}
*/
	
    void afficher();
	
	// --------- Fonctions liées à convol.h, fcts_lk3.h, crea_tiff_3.h ------
#if INCLUDE_CONVOL
	Image* correlation_lk(Image& reference, float seuil_ref);
#endif
#if INCLUDE_FCTS_LK3
	static Image* dessinerMasqueDeSoleil(int diametre_en_pixel);
#endif
#if INCLUDE_INTERPOL
	Image* interpolerAutourDeCePoint(int l, int c);
	Image* interpolerAutourDeCePoint(int l, int c, float pas_interp, float marge);
	void maxParInterpolation(double *l, double *c);
#endif

};

#endif /* defined(__climso_auto__picture__) */
