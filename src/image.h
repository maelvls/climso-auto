//
//  image.h
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

/**
 * J'ai écrit cette classe pour simplifier l'utilisation des nombreuses fonctions sur des images, du type
 * uneFonction(double** tab_entree, tab_entree_larg, tab_entree_haut, double** tab_sortie, int tab_sortie_larg, int tab_sortie_long)
 *
 * Une image équivaut à un tableau 2D de "double" (pour rester compatible avec les fonctions existantes)
 * J'ai choisit la convention "matrice" : un point est désigné par point(ligne,colonne)
 * en partant du point extrème nord-ouest. D'autres représentations conseillent point(x,y)
 * mais je n'ai pas pris cette convention.
 *
 * NOTE1: Concernant la représentation en mémoire de l'image, j'hésite encore beaucoup entre la représentation
 * linéaire (un seul tableau de "double" avec les lignes les unes à la suite des autres) et la représentation
 * matricielle, c'est à dire un tableau de tableaux de "double".
 * 			-> j'ai choisi une représentation linéaire, mais non-compatible avec les fonctions antérieures
 * 			mais par contre compatibles avec les fonctions d'affichage Qt par exemple
 * NOTE3: Après pas mal de recul, je pense que cette représentation linéaire n'aide pas lors des optimisations
 * car pour le calcul (type correlation), on manipule des pointeurs pour optimiser... Or, ça veut dire qu'on
 * utilise plus les getters et donc la classe telle qu'elle est définie est moins "solide".
 *
 * NOTE2: Cette classe a un gros soucis avec la création multiple d'objets lourds : à chaque fois qu'on traite
 * une image, on crée un nouvel objet en mémoire. Dans une boucle, cela ralenti le processus...
 *
 * NOTE3: Pourquoi utiliser des doubles alors que de simples 16-bits (ushort par exemple) sont suffisants
 * et codent pour 65535 tons ? En fait, on a besoin de ces doubles uniquement dans le cas de la correlation.
 * Du coup, on pourrait juste utiliser un tableau de double pour la correlation et ensuite copier dans l'image
 * résultat...
 *
 */

#ifndef __climso_auto__picture__
#define __climso_auto__picture__

#define INCLUDE_INTERPOL	1	// Fonctions contenues dans interpol.c

#define INCLUDE_TIFF		1 	// Si 1, alors -L/usr/local/lib et -ltiff
#define INCLUDE_SBIGCAM		1
#define INCLUDE_SBIGIMG		1 	// Si 1, alors	(MAC) -F/Library/Frameworks et -framework SBIGUDrv (selon installation)
								//				(LINUX) -L/usr/local/lib et -lsbigudrv (selon installation)

#ifndef DEBUG
#define DEBUG 1
#endif

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
#if INCLUDE_INTERPOL
	#include "interpol.h"
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
    Image* correlation_simple(Image& p, float seuil_ref);
    void tracerDonut(int l_centre, int c_centre, double freq_min, double marge_int, double freq_max, double marge_ext);
    static Image* tracerFormeSoleil(int diametre);
    Image* correlation_rapide(Image& reference, float seuil_ref);
    Image* convoluer(const int *noyau, int taille);
    Image* convoluerParDerivee();
    Image* deriveeCarre();
    double calculerHauteurRelativeAutour(int l, int c);

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
