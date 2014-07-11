//
//  diametre_soleil.c
//  climso-auto
//
//  Created by Maël Valais on 14/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//
//  Historique des modifs
//  2014_04_15 LK : indiquees par "/**/" dans la marge
//  2014_04_15 MV : ajout du parametre d'entree "time_t t" pour diametreSoleilRadian

#include <stdio.h>
#include <math.h>
#include "diametre_soleil.h"


/// Définition des propriétés de l'image de référence


#if DEBUG

// Nouvelle référence pour débug :
#define IMAGE_REFERENCE_DIAMETRE 	1616 // En pixels
#define IMAGE_REFERENCE_ANNEE		2014 // En UTC (GMT)
#define IMAGE_REFERENCE_MOIS		4
#define IMAGE_REFERENCE_JOUR		21
#define IMAGE_REFERENCE_HEURE		12

#else

// Image de référence calculée par LKoechlin
#define IMAGE_REFERENCE_DIAMETRE 	1638 // En pixels
#define IMAGE_REFERENCE_ANNEE		2014 // En UTC (GMT)
#define IMAGE_REFERENCE_MOIS		7
#define IMAGE_REFERENCE_JOUR		14
#define IMAGE_REFERENCE_HEURE		12

#endif

/**
 * Donne le diametre du soleil en seconde d'arc
 * @param t La date time_t (cf manuel time.h ou ctime ou man 3 time)
 * @return l'angle en seconde d'arc
 */
double diametreSoleilSecArc(time_t t) {
    const double pi = 3.141592653589793238462643383279502884197; // et plus si ca vous amuse
    const double radsec = 3600*180/pi; // nombre de secodes d'arc dans un radian
    
    const double diametreSoleil = 1.392E09; // en m
    const double a =    149597870E3;   // Demi grand-axe de l'orbite de la Terre, en m
    const double e =    0.01671;       // Excentricité de l'orbite de la Terre
    const double anneeDuree = 365.2564;     // Année sidérale moyenne en jours
    
    struct tm t_0_tm; // ses champs contiendront les an,mois,jour,h,mn,s de la date du périhelie : 2014_01_04-12h00mn00s UTC (UTC=GMT)
    t_0_tm.tm_year = 114; // et pas 2014 car le calendrier Unixien commence en 1900
    t_0_tm.tm_mon = 0; // 0 pour Janvier et pas 1 car les indices commencent a 0 chez les Unixiens
    t_0_tm.tm_mday = 4; // 1-31 pour les jours
    t_0_tm.tm_hour = 12; // 0-23
    t_0_tm.tm_min = 0; // 0-59
    t_0_tm.tm_sec = 0; // 0-59
    time_t t_0_time = mktime(&t_0_tm); // 't_0_time' contiendra cette date du perihelie traduite en secondes
    
    
    double seconds = difftime(t, t_0_time); // nb de secondes ecoule entre maintenant et 't_0'
    double nb_jours = seconds/3600/24;        // nb de jours    ecoule entre maintenant et 't_0', pas forcement entier

#if DEBUG
    printf("Nb jours depuis le perihelie2014=%.4f \n", nb_jours);
#endif
    
    double M = 2*pi* nb_jours / anneeDuree; // Anomalie moyenne (orbite circulaire).
    double U1 = M+e*sin(M); // Anomalie excentrique U1
    double U2 = M+e*sin(U1); // Anomalie excentrique U2
    // M, U1 et U2 vont depasser 2*pi a partir de 2015, mais pas de pb : la fonction tan fait un 'modulo 2*pi'
    /**/    double nu = 2*atan(tan(U2/2)*sqrt((1+e)/(1-e))); // Anomalie vraie
    
    double distanceTerreSoleil = a*(1-e*e) / (1+e*cos(nu));

#if DEBUG
    printf("M=%.10f\n"
           "U1=%.10f\n"
           "U2=%.10f\n"
           "nu=%.10f\n"
           "Angle en radians=%.10f\n" , M, U1, U2, nu,diametreSoleil/distanceTerreSoleil);

#endif
    double angle_apparent = diametreSoleil / distanceTerreSoleil * radsec; // en radians, approx petits angles
    // Sachant que alpha << 1 => alpha ~ tan(alpha).
    
#if DEBUG
    printf("Distance Soleil_Terre aujourd'hui =%.10f, angle en secondes d'arc=%.10f\n",
           distanceTerreSoleil, angle_apparent);
#endif

    return angle_apparent;
}

/**
 * Calcul du diamètre en pixels du Soleil à la date t grâce à la fonction
 * 	diametreSoleilRadian, une taille et un angle de référence
 * 	que nous avons calculée à partir d'une image existante
 * @param t La date
 * @return le diamètre du Soleil en pixels
 */
int diametreSoleilPixels(time_t t) {
    const int diametreReference = IMAGE_REFERENCE_DIAMETRE; // d'après une image I
    struct tm tmRef;
    tmRef.tm_year=IMAGE_REFERENCE_ANNEE - 1900; // années depuis 1900
    tmRef.tm_mon=IMAGE_REFERENCE_MOIS - 1; // 0-11
    tmRef.tm_mday=IMAGE_REFERENCE_JOUR; // 1-31
    tmRef.tm_hour=IMAGE_REFERENCE_HEURE; // 0-23
    tmRef.tm_min = 0; // 0-59
    tmRef.tm_sec = 0; // 0-59
    time_t tRef = mktime(&tmRef);// La date (en sec) de prise de vue de l'image I (le 14 juillet 2014 à 12h30m00sec UTC (UTC=GMT))
    return diametreReference * diametreSoleilSecArc(t)/diametreSoleilSecArc(tRef);
}


/*
int main(int argc, const char *argv[])
{
    struct tm t; t.tm_year=2014-1900; t.tm_mon=3-1; t.tm_mday=1; t.tm_hour=12;

	printf("Diametre au jour t : %.10f\n",diametreSoleilRadian(mktime(&t)));
    printf("Taille en pixels : %d\n",diametreSoleilPixels());
	return 0;
}
*/

