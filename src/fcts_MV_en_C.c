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

int diffDates(Date fin, Date deb) {
    double duree;
    time_t t = time(NULL);
    struct tm *tm_fin = localtime(&t), *tm_deb = localtime(&t);
    tm_fin->tm_mday = fin.jour;
    tm_fin->tm_mon = fin.mois-1;
    tm_fin->tm_year = fin.annee-1900;
    
    tm_deb->tm_mday = deb.jour;
    tm_deb->tm_mon = deb.mois-1;
    tm_deb->tm_year = deb.annee-1900;
    
    time_t tm_time_fin = mktime(tm_fin);
    time_t tm_time_deb = mktime(tm_deb);
    
    printf("Debut : %sFin : %s",ctime(&tm_time_deb),ctime(&tm_time_fin));
    
    duree = difftime(tm_time_fin, tm_time_deb); // en secondes
    duree = duree/86400; // passage en jours
    return duree;
}
/*=====================================================
 Donne le diametre du soleil en radians
 Retours: l'angle en radian modulo 2pi
 =====================================================*/
double diametreSoleilRadian(int jour, int mois, int annee) {
    const double diametreSoleil = 1.392E09;
    const double pi =   3.14159265359;
    const double a =    1.496010E11; // Demi grand-axe Terre
    const double e =    0.01671; // Excentricité Terre
    const double anneeJours = 365.2564; // Année sidérale moyenne en jours
    Date dateRef; dateRef.jour=4; dateRef.mois=1; dateRef.annee=2014;
    
    Date date; date.jour=jour; date.mois=mois; date.annee=annee;
    short diffJours = diffDates(dateRef,date);

    double M = 2*pi*diffJours/anneeJours; // Anomalie moyenne (orbite circulaire)
    double U1 = M+e*sin(M); // Anomalie excentrique U1
    double U2 = M+e*sin(U1); // Anomalie excentrique U2
    printf("U2=%.10f\n",U2);
    double nu = 2*atan(tan(U2/2)*sqrt((1+e)/1-e)); // Anomalie vraie
    double distanceTerreSoleil = a*(1-e*e) / (1+e*cos(nu));
    // Sachant que l'angle alpha << 1, alpha approche tan(alpha). Ainsi :
    return diametreSoleil/distanceTerreSoleil;
}

int main(int argc, const char *argv[])
{
	Date fin; fin.mois = 10; fin.jour = 10; fin.annee=2014;
    Date deb; deb.mois = 6; deb.jour = 6; deb.annee=2014;
    
    printf("Difference en jours : %d\n",diffDates(fin, deb));

    printf("Le %d/%d  : %.20f\n",fin.jour,fin.mois,diametreSoleilRadian(fin.jour, fin.mois, fin.annee));
	return 0;
}