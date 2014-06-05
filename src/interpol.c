/*   11/2/1999 EG et LK   fonctions d'interpolations de Neville-Aitken
						sur des tableaux de réels 1D et 2D
	18/6/2007 LK		correction d'un bug :
						remplacement de (round) par (floor) dans le calcul de l'indice de depart
						pour les interpolations 2D.
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "interpol.h"


static inline int min (int a, int b) {return (a < b) ? a:b ;}
static inline int max (int a, int b) {return (a > b) ? a:b ;}


//--------------------------------------------------------------------------
//	Interpolation 1D (Neville-Aitken) sur quatre points
//		parametres d'entree:	f: valeurs de la fonction pour des arguments entiers
//								alpha: point ou on veut l'interpolation
//		donne en retour:		la valeur interpolee
//--------------------------------------------------------------------------
double it_pol_neville_s4 (double *f, double alpha)
{
	static double d [4][4];

	d[0][1] = ( alpha	  * f[1] - (alpha-1) * f[0] );
	d[1][1] = ( (alpha-1) * f[2] - (alpha-2) * f[1] );
	d[2][1] = ( (alpha-2) * f[3] - (alpha-3) * f[2] );

	d[0][2] = ( (alpha  ) * d[1][1] - (alpha-2) * d[0][1] ) / 2;
	d[1][2] = ( (alpha-1) * d[2][1] - (alpha-3) * d[1][1] ) / 2;

	return    ( (alpha  ) * d[1][2] - (alpha-3) * d[0][2] ) / 3;
}
//--------------------------------------------------------------------------
//	Interpolation 1D (Neville-Aitken) sur trois points
//		parametres d'entree:	f: valeurs de la fonction pour des arguments entiers
//								alpha: point ou on veut l'interpolation
//		donne en retour:		la valeur interpolee
//--------------------------------------------------------------------------
double it_pol_neville_s3 (double *f, double alpha)
{
	double d_01 = ( alpha	 * f[1] - (alpha-1) * f[0] );
	double d_11 = ( (alpha-1) * f[2] - (alpha-2) * f[1] );

	return ( alpha * d_11 - (alpha-2) * d_01 ) / 2;
}
//--------------------------------------------------------------------------
//	Interpolation 1D sur deux points
//		parametres d'entree:	f: valeurs de la fonction pour deux arguments entiers
//								alpha: point ou on veut l'interpolation
//		donne en retour:		la valeur interpolee
//--------------------------------------------------------------------------
inline double it_pol_neville_s2 (double *f, double alpha)
{
	return ( alpha	 * f[1] - (alpha-1) * f[0] );
}
//--------------------------------------------------------------------------
//      Interpolation 2D (Neville-Aitken) sur 4 fois 4 points
//      parametres d'entree: n, m: dimensions des tableaux
//                           f: table de la fonction 2D
//                           y_1, x_1: point (x,y) ou on veut l'interpolation
//     donne en retour:		 valeur interpolee
//--------------------------------------------------------------------------
double it_pol_neville2D_s4 (
	int n,				// taille du tableau source en y
	int m,				// taille du tableau source en x
	double **f,			// tableau source
	double y_1,			// coordonnée y du point pour lequel on veut une val interpolée
	double x_1)			// coordonnée x du point pour lequel on veut une val interminpolée
{
	int i;
	static double	x_interm [4];	// tableau intermediaire declare en statique

	// ---------------- determination de la zone a selectionner en X ------------------
	int indice_x1_inf = (int)floor (x_1) - 1;	// centrer la zone d'interpol autour de x_1
	indice_x1_inf = max (indice_x1_inf, 0);		// butee inf
	indice_x1_inf = min (indice_x1_inf, m-4);	// butee sup
	// ---------------- determination de la zone a selectionner en Y ------------------
	int indice_y1_inf = (int)floor (y_1) - 1;	// centrer la zone d'interpol autour de y_1
	indice_y1_inf = max (indice_y1_inf, 0);		// butee inf
	indice_y1_inf = min (indice_y1_inf, n-4);	// butee sup
	// ------ boucle sur les x : interpols en y mises dans un tableau temporaire ------
	for (i=0; i<4; i++)
	{
		x_interm[i] = it_pol_neville_s4 ( &(f [indice_y1_inf +i][indice_x1_inf]),
										x_1 - indice_x1_inf);
	}
	return it_pol_neville_s4 (x_interm, y_1 - indice_y1_inf);// nterpol finale en y
}
//--------------------------------------------------------------------------
//      Interpolation 2D (Neville-Aitken) sur 3 fois 3 points
//      parametres d'entree: n, m: dimensions des tableaux
//                           f: table de la fonction 2D
//                           y_1, x_1: point (x,y) ou on veut l'interpolation
//     donne en retour:		 valeur interpolee
//--------------------------------------------------------------------------
double it_pol_neville2D_s3 (
	int n,				// taille du tableau source en y
	int m,				// taille du tableau source en x
	double **f,			// tableau source
	double y_1,			// coordonnée y du point pour lequel on veut une val interpolée
	double x_1)			// coordonnée x du point pour lequel on veut une val interpolée
{
	int i;
	static double	x_interm [3];	// tableau intermediaire declare en statique

	// ---------------- determination de la zone a selectionner en X ------------------
	int indice_x1_inf = (int)floor (x_1) - 1;	// centrer la zone d'interpol autour de x_1
	indice_x1_inf = max (indice_x1_inf, 0);		// butee inf
	indice_x1_inf = min (indice_x1_inf, m-3);	// butee sup
	 
	// ---------------- determination de la zone a selectionner en Y ------------------
	int indice_y1_inf = (int)floor (y_1) - 1;	// centrer la zone d'interpol autour de y_1
	indice_y1_inf = max (indice_y1_inf, 0);		// butee inf
	indice_y1_inf = min (indice_y1_inf, n-3);	// butee sup
	
	// ------ boucle sur les x : interpols en y mises dans un tableau temporaire ------
	for (i=0; i<3; i++)
	{
		x_interm[i] = it_pol_neville_s3 ( &(f [indice_y1_inf +i][indice_x1_inf]),
										x_1 - indice_x1_inf);
	}
	return it_pol_neville_s3 (x_interm, y_1 - indice_y1_inf);// nterpol finale en y
}
//--------------------------------------------------------------------------
//      Interpolation 2D sur 2 fois 2 points
//      parametres d'entree: n, m: dimensions des tableaux
//                           f: table de la fonction 2D
//                           y_1, x_1: point (x,y) ou on veut l'interpolation
//     donne en retour:		 valeur interpolee
//--------------------------------------------------------------------------
double it_pol_neville2D_s2 (
	int n,				// taille du tableau source en y
	int m,				// taille du tableau source en x
	double **f,			// tableau source
	double y_1,			// coordonnée y du point pour lequel on veut une val interpolée
	double x_1)			// coordonnée x du point pour lequel on veut une val interpolée
{
	static double	x_interm [2];	// tableau intermediaire declare en statique

	// ---------------- determination de la zone a selectionner en X ------------------
	int indice_x1_inf = (int)floor (x_1);		// centrer la zone d'interpol autour de x_1
	indice_x1_inf = max (indice_x1_inf, 0);		// butee inf
	indice_x1_inf = min (indice_x1_inf, m-2);	// butee sup
	 
	// ---------------- determination de la zone a selectionner en Y ------------------
	int indice_y1_inf = (int)floor (y_1);		// centrer la zone d'interpol autour de y_1
	indice_y1_inf = max (indice_y1_inf, 0);		// butee inf
	indice_y1_inf = min (indice_y1_inf, n-2);	// butee sup
	
	// ------ boucle sur les x : interpols en y mises dans un tableau temporaire ------
		x_interm[0] = it_pol_neville_s2 ( &(f [indice_y1_inf]   [indice_x1_inf]), x_1 - indice_x1_inf);
		x_interm[1] = it_pol_neville_s2 ( &(f [indice_y1_inf +1][indice_x1_inf]), x_1 - indice_x1_inf);
	return it_pol_neville_s2 (x_interm, y_1 - indice_y1_inf);// interpol finale en y
}
