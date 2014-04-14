//
//  fcts_MV.c
//  climso-auto
//
//  Created by Maël Valais on 14/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fcts_MV_en_C.h"

int estBissextile(Date d) {
    return (d.annee % 4 == 0 && d.annee % 100 != 0) || d.annee % 400 == 0;
}
short numeroDuJourDansAnnee(Date d) {
    short numeroDuJour = d.jour;
    short m;
    for (m = d.mois-1; m > 0; m--) { // On ne prend pas le mois en cour
        switch (m) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                numeroDuJour += 31; break;
            case 2: numeroDuJour += estBissextile(d)?29:28; break;
            case 4: case 6: case 9: case 11:
                numeroDuJour += 30; break;
            default: break;
        }
    }
    return numeroDuJour;
}

/* =====================================================
 * Donne le nombre de jours entre les deux dates selon d1 - d2 
 *
 * Entrées: d1, d2 : les deux dates
 * Returns: le nombre de jours de différence modulo le nombre de jours dans l'année
 * =====================================================*/
short differenceEntreJoursDuneAnnee(Date d1, Date d2) {
    short diff = numeroDuJourDansAnnee(d1) - numeroDuJourDansAnnee(d2);
    if (diff < 0) {
        diff = estBissextile(d2)?366:365 + diff;
    }
    return diff;
}
/*=====================================================
 Donne le diametre du soleil en radians
 Retours: l'angle en radian modulo 2pi
 =====================================================*/
double diametreSoleilRadian(int jour, int mois) {
    static double pi =   3.14159265359;
    static double diametreSoleil = 1.392E09;
    static double a =    1.496010E11; // Demi grand-axe Terre
    static double e =    0.01671; // Excentricité Terre
    static double annee = 365.2564; // Année sidérale moyenne en jours
    static Date dateRef; dateRef.jour=4; dateRef.mois=1;
    
    Date date; date.jour=jour; date.mois=mois;
    short diffJours = differenceEntreJoursDuneAnnee(dateRef,date);

    double M = 2*pi*diffJours/annee; // Anomalie moyenne (orbite circulaire)
    double U1 = M+e*sin(M); // Anomalie excentrique U1
    double U2 = M+e*sin(U1); // Anomalie excentrique U2
    double v = 2*atan(tan(U2/2)*sqrt((1+e)/1-e)); // Anomalie vraie
    double distanceTerreSoleil = a*(1-e*e) / (1+e*cos(v));
    // Sachant que l'angle alpha << 1, alpha approche tan(alpha). Ainsi :
    return diametreSoleil/distanceTerreSoleil;
}

int main(int argc, const char *argv[])
{
	Date a; a.mois = 1; a.jour = 4;
    Date b; b.mois = 1; b.jour = 3;
    printf("Diff entre a et b : %d", differenceEntreJoursDuneAnnee(a, b));
    printf("Le %d/%d  : %.20f",a.jour,a.mois,diametreSoleilRadian(a.jour, a.mois));
	return 0;
}