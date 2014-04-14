/* 	fonctions reprises de LK

	allocation memoire tableaux 2D 
	
	v3: juste pour harmoniser avec mes autres prgms: alloc_mat_2D(nbpts_x,nbpts_y,...)
		et intro de calc_min_all, correction mineure de calc_max_all
		
	ajout de calc_max_crop, calc_min_crop 	
	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "fcts_LK3.h"


inline int min (int a, int b) {return (a < b) ? a:b ;}
inline int max (int a, int b) {return (a > b) ? a:b ;}


//-----------------------------------------------------------------------------
//		Allocation dynamique d'un tableau a deux dimensions
//	Paramètre d'entrée :	size_v, size_h : monbre de lignes et de colonnes
//							size_v <=> nbpts_y et size_h <=> nbpts_x
//							el_size : taille (sizeof) d'un élément du tableau
// Retourne : un pointeur de void sur le tableau à deux dimensions
// pour appeler ensuite un element du tableau 2D on appellera : mat[i_colone][i_ligne]
//-----------------------------------------------------------------------------
void** alloc_mat_2D (int size_h, int size_v, int el_size) {
	int iv, i_del;
	void **mat_ptr, *ligne_ptr;
		
	mat_ptr = (void **)malloc ((size_v +1) * sizeof(void *));
	if (mat_ptr == NULL) return NULL;
	for (iv = 0 ; iv < size_v ; iv++) {
		ligne_ptr = malloc (size_h * el_size);
		if (ligne_ptr != NULL) mat_ptr [iv] = ligne_ptr;
		else {
			for (i_del = 0 ; i_del<iv ; i_del++)  free (mat_ptr [i_del]);	/* si échec,tout libérer et halt */
			printf ("alloc_mat_2D : pas assez de mem\n");
			exit (1);
		}
	}
	mat_ptr [size_v] = NULL;							/* marque la fin pour free */
	return mat_ptr;
}

//-----------------------------------------------------------------------------
//		mettre a 0 un tableau
//		paramètres d'entrée : data : tableau 2D
//-----------------------------------------------------------------------------
void raz_mat_2D (double **data, int size_h, int size_v)
{	int	ih, iv;
    
	for (iv = 0 ; iv<size_v ; iv++)
        for (ih = 0 ; ih<size_h ; ih++)
            data[iv][ih] = 0.0;	
}



//-----------------------------------------------------------------------------
//		Libération mémoire d'un tableau a deux dimensions
//	modif DS du 20-10-06
//-----------------------------------------------------------------------------
/*void free_mat_2D (void **mat_ptr) {
	int i = 0;
	
	while (mat_ptr [i] != NULL)  free (mat_ptr [i++]);		// fin indiquée par NULL
	free (mat_ptr);
	}//*/

void free_mat_2D (void **mat_ptr) {
	int i = 0;
	
	while (mat_ptr [i] != NULL)// fin indiquée par NULL 
	{	free (mat_ptr [i]);
		i++;
	}
	free (mat_ptr[i]);
	
	free (mat_ptr);
}

//-----------------------------------------------------------------------------
//	recopie d'une partie d'un tableau 2D vers un autre de forme différente et plus petit
//          La zone rectangle recopiée a la forme du tableau destination
//		param d'entrée :	data_src: tab 2D de dimensions   size_src_i * size_src_j
//                          org_x, org_y : indices de départ dans le tableau à recopier vers la destination
//							size_dest_i, size_dest_j : dimensions du rectangle à recopier, 
//		param de sotrie :	data_dest: tab 2D destination de dimensions size_dest_i * size_dest_j
//-----------------------------------------------------------------------------
void recopie (double **data_src, int size_src_j, int size_src_i, int org_j, int org_i, double **data_dest, int size_dest_j, int size_dest_i)
{
	// initialise le tableau data_dest a 0
	{
        for (int i = 0; i < size_dest_i; i++)
        for (int j = 0; j < size_dest_j; j++)
            data_dest[i][j] = 0.;
    }    
    // sécurités
    if (org_i < 0) org_i = 0;
    if (org_j < 0) org_j = 0;
    if (size_dest_i > size_src_i - org_i -1) size_dest_i = size_src_i - org_i -1;
    if (size_dest_j > size_src_j - org_j -1) size_dest_j = size_src_j - org_j -1;
    if (size_dest_i < 0) size_dest_i = 0;    
    if (size_dest_j < 0) size_dest_j = 0;    
    
	// recopie depuis data_src vers data_dest, a partir des coordonnnes org_x et org_y
    int i_src = org_i;
    for (int i_dest = 0; i_dest < size_dest_i; i_dest++)
    {
        int j_src = org_j;
        for (int j_dest = 0; j_dest < size_dest_j; j_dest++)
        {
            data_dest[i_dest][j_dest] =  data_src[i_src][j_src++];
        }
        i_src++;
    }
}

//-----------------------------------------------------------------------------
//	binage 2x2 d'un tableau 2D
//		param d'entrÈe :	data_brut : tab 2D de dimensions size_x*2, size_y*2
//							size_x, size_y,
//		param de sotrie :	data : tab 2D de dim size_x, size_y
//-----------------------------------------------------------------------------
void bin_2x2 (double **data_brut, double **data, int size_x, int size_y)
{
	int i,j, k, m;
	double **data_temp = (double**)  alloc_mat_2D (size_x, size_y, sizeof(double));
	if (data_temp == NULL)	exit (0);
    raz_mat_2D (data_temp, size_x, size_y);
	
	for (i=k = 0 ; i<size_y ; i++, k+=2)
        for (j=m = 0 ; j<size_x ; j++, m+=2) data_temp[i][j] = data_brut [k][m]
            +data_brut [k]  [m+1]
            +data_brut [k+1][m]
            +data_brut [k+1][m+1];
    
	for (i = 0 ; i<size_y ; i++)
        for (j = 0 ; j<size_x ; j++) 
            data[i][j] = data_temp[i][j] /4.;
    
	free_mat_2D ((void**)data_temp);
}


//-----------------------------------------------------------------------------
//		caculer le max  a l'intérieur d'un tableau
//		paramètres d'entrée : data : tableau 2D
//		retourne : le max
//-----------------------------------------------------------------------------
double calc_max_all (double **data, int size_h, int size_v)
{	int	ih, iv;
	double max = data[0][0];
    
	for (iv = 0 ; iv<size_v ; iv++)
        for (ih = 0 ; ih<size_h ; ih++)
            if (data[iv][ih] > max)
            {	max = data[iv][ih];
            }
	return max;
}

//-----------------------------------------------------------------------------
//		caculer l'argument du max à l'intérieur d'un tableau et la valeur du max
//		paramètres d'entrée : 
//            data : tableau 2D
//            *xymax : pointeur sur une structure 'point' où sera mis l'argument (coordonnées x,y) du maximum
//		retourne : le max
//-----------------------------------------------------------------------------
double calc_arg_max_all (double **data, point *xymax, int size_h, int size_v)
{
    int	ih, iv;
	double max = data[0][0];

    xymax->x = 0;
    xymax->y = 0;
    
	for (iv = 0 ; iv<size_v ; iv++)
        for (ih = 0 ; ih<size_h ; ih++)
            if (data[iv][ih] > max)
            {
                max = data[iv][ih];
                xymax->x = ih;
                xymax->y = iv;
            }
	return max;
}

//-----------------------------------------------------------------------------
//		caculer la moyenne  a l'intérieur d'un tableau
//		paramètres d'entrée : data : tableau 2D
//		retourne : la moyenne
//-----------------------------------------------------------------------------
double calc_moyenne_all (double **data, int size_x, int size_y)
{	int	i, j;
	double somme = 0;
    
	for (i = 0 ; i<size_y ; i++)
        for (j = 0 ; j<size_x ; j++)
            somme += data[i][j];        
    
	return somme/size_x/size_y;
}

//-----------------------------------------------------------------------------
//		caculer la moyenne  a l'intérieur d'un tableau
//		paramètres d'entrée : 
//          data : tableau 2D
//          val : valeur à ajouter en chaque point
//-----------------------------------------------------------------------------
void add_val (double **data, double val, int size_x, int size_y)
{	int	i, j;
    
	for (i = 0 ; i<size_y ; i++)
        for (j = 0 ; j<size_x ; j++)
            data[i][j] += val;        
}

//-----------------------------------------------------------------------------
//	Calcul du Laplacien d'un tableau 2D scalaire : d^2f/dx^2 + d^2f/dy^2
//	Paramètre d'entrée : 	
//      data :              tableau 2D
//      size_x, size_y :    dimensions du tableau
//	Paramètre de sortie :	laplacien,	tableau 2D
//-----------------------------------------------------------------------------
void calc_laplacien (double **data, double **laplacien, int size_x, int size_y) 
{
	int i,j;
	
    { // mise à 0 initiale du tableau
        for (i = 0 ; i<size_y ; i++)
        for (j = 0 ; j<size_x ; j++)
            laplacien[i][j] = 0.0;	
    }
    
	for (i=1 ; i<size_y-1 ; i++)
		for (j=1 ; j<size_x-1 ; j++)
		{
			laplacien[i][j] = 4 * data[i][j] - data[i-1][j] - data[i+1][j] - data[i][j-1] - data[i][j+1];
		}	
	for (i = 0 ; i<size_y ; i++)		// interpolation aux bords =
	{	
		laplacien[i][0] =		0.5 * (laplacien[i][1] + laplacien[i][size_x-2]);
		laplacien[i][size_x-1] = laplacien[i][0];
	}
	for (j = 0 ; j<size_x ; j++)		// interpolation aux bords ||
	{	
		laplacien[0][j] =		0.5 * (laplacien[1][j] + laplacien[size_y-2][j]);
		laplacien[size_y-1][j] = laplacien[0][j];	
	}
}


//-----------------------------------------------------------------------------
//	Calcul la somme de : (valeur absolue de dérivée partielle en x + val.abs.deriv.partielle en y)
// d'un tableau 2D scalaire : |df/dx| + |df/dy|
//	Paramètre d'entrée : 	
//      data :              tableau 2D
//      size_x, size_y :    dimensions du tableau
//	Paramètre de sortie :	somme des dérivées x et y,	tableau 2D
//-----------------------------------------------------------------------------
void calc_abs_deriv (double **data, double **absderiv, int size_x, int size_y)
{
	int i,j;

    // mise à 0 initiale du tableau
    for (i = 0 ; i<size_y ; i++)
    for (j = 0 ; j<size_x ; j++)
        absderiv[i][j] = 0.0;	

    for (i=1 ; i<size_y-1 ; i++)
    for (j=1 ; j<size_x-1 ; j++)
        absderiv[i][j] = sqrt( (data[i][j] - data[i-1][j]) * (data[i][j] - data[i-1][j]) 
                       +       (data[i][j] - data[i][j-1]) * (data[i][j] - data[i][j-1]) );
}


//-----------------------------------------------------------------------------
//		caculer le min  a l'intérieur d'un tableau
//		paramètres d'entrée : data : tableau 2D
//		retourne : le min
//-----------------------------------------------------------------------------
double calc_min_all (double **data, int size_x, int size_y)
{	int	i, j;
	double min = data[0][0];

	for(i = 0 ; i<size_y ; i++)
 	{	for (j = 0 ; j<size_x ; j++)
		{	if (data[i][j] < min)  
			{	min = data[i][j];
			}
		}
	}
	return min;
}


//-----------------------------------------------------------------------------
//		caculer le max  a l'intérieur d'un vecteur
//		paramètres d'entrée : data : tableau 1D
//							  size : dimension du vecteur
//		retourne : le max
//-----------------------------------------------------------------------------

double calc_max_all_1D(double *data, int size)
{	int i;
	double max;
	
	max = data[0];
	
	for(i=1;i<size;i++)
	{	if( data[i] > max)
		{	max = data[i];
		}
	}
	
	return max;
}	

int calc_max_all_1D_pos(double *data, int size)
{	int i;
	double max;
	int retour;
	
	max = data[0];
	retour = 0;
	
	for(i=1;i<size;i++)
	{	if( data[i] > max)
		{	max = data[i];
			retour = i;
		}
	}
	
	return retour;
}	

//-----------------------------------------------------------------------------
//		caculer le min  a l'intérieur d'un vecteur
//		parametres d'entrée : data : tableau 1D
//							  size : dimension du vecteur
//		retourne : le min
//-----------------------------------------------------------------------------

double calc_min_all_1D(double *data, int size)
{	int i;
	double min;
	
	min = data[0];
	
	for(i=1;i<size;i++)
	{	if( data[i] < min)
		{	min = data[i];
		}
	}
	
	return min;
}

int calc_min_all_1D_pos(double *data, int size)
{	int i;
	double min;
	int retour;
	
	min = data[0];
	retour = 0;
	
	for(i=1;i<size;i++)
	{	if( data[i] < min)
		{	min = data[i];
			retour = i;
		}
	}
	
	return retour;
}	

//-----------------------------------------------------------------------------
//		caculer le max  a l'intérieur d'une zone d'un tableau
//		paramètres d'entrée : data : tableau 2D
//					min_x,max_x,min_y,max_y: zone du tableau ou on cherche
//		retourne : le max
//----------------------------------------------------------------------------
double calc_max_crop (double **data,int min_x, int max_x, int min_y, int max_y)
{	int	i, j;
	double max = data[min_y][min_x];

	for (i = min_y ; i<=max_y ; i++)
	{	for (j = min_x ; j<=max_x ; j++)
		{	if (data[i][j] > max)  
			{	max = data[i][j];
			}
		}
	}
	return max;
	
}

//-----------------------------------------------------------------------------
//		calculer le min  a l'intérieur d'une zone d'un tableau
//		paramètres d'entrée : data : tableau 2D
//					min_x,max_x,min_y,max_y: zone du tableau ou on cherche
//		retourne : le min
//----------------------------------------------------------------------------
double calc_min_crop (double **data,int min_x, int max_x, int min_y, int max_y)
{	int	i, j;
	double min = data[min_y][min_x];

	for (i = min_y ; i<=max_y ; i++)
 	{	for (j = min_x ; j<=max_x ; j++)
		{	if (data[i][j] < min)  
			{	min = data[i][j];
			}
		}
	}
	return min;
	
}



//-----------------------------------------------------------------------------
//		normalisation d'un tableau
//		params d'entree :	image_src,					tableau qu'on va normaliser
//							norme,						normalisation voulue
//							size_src_h, int size_src_v	taille images source et convoluee
//		param de sotrie :	image_dest					le tableau apres normalisation
//-----------------------------------------------------------------------------
void normalise	(double **image_src, double **image_dest, int size_src_h, int size_src_v, double norm)
{
	double max = calc_max_all (image_src, size_src_h, size_src_v);
	double norm_S_max = norm / max;
	
	for (int v = 0 ; v<size_src_v ; v++)
	for (int h = 0 ; h<size_src_h ; h++)
		image_dest [v][h] = image_src [v][h] * norm_S_max;
}

//-----------------------------------------------------------------------------
//		mise a la puissance 2 d'un tableau
//		params d'entree :	image_src,					tableau qu'on va normaliser
//							size_src_h, int size_src_v	taille images source et convoluee
//		param de sotrie :	image_src					le meme tableau qu'en entree
//-----------------------------------------------------------------------------
void carre_tab_2D (double **image_src, int size_src_h, int size_src_v)
{	
	for (int v = 0 ; v<size_src_v ; v++)
	for (int h = 0 ; h<size_src_h ; h++)
		image_src [v][h] *= image_src [v][h];
}

//-----------------------------------------------------------------------------
//		mise a la puissance 4 d'un tableau
//		params d'entree :	image_src,					tableau qu'on va normaliser
//							size_src_h, int size_src_v	taille images source et convoluee
//		param de sotrie :	image_src					le meme tableau qu'en entree
//-----------------------------------------------------------------------------
void pow4_tab_2D (double **image_src, int size_src_h, int size_src_v)
{	
	for (int v = 0 ; v<size_src_v ; v++)
	for (int h = 0 ; h<size_src_h ; h++)
	{
		image_src [v][h] *= image_src [v][h];
		image_src [v][h] *= image_src [v][h];
	}
}

//-----------------------------------------------------------------------------
/*
Draws a doughnut-shaped region that can be used for example as a spatial frequency filter.  (traduit d'un code LK 2010 en python)
 in:
 filtre             # un pointeur vers le tab 2D qui contiendra la galette
 size_h, int size_v # sizes of the 2D array in which to draw
 i_centre, j_centre # Y and X coordinates of center of doughnut in the array
 freq_min           # Inner radius of doughnut. If freq_min = marge_int = 0, there is no central hole : a disc is drawn.
 marge_int          # width in pixels of the inner margin of doughnut (Hanning smooth)
 freq_max           # Outer radius of doughnut
 marge_ext          # width in pixels of the outer margin of doughnut (Hanning smooth)
 
 out :  for example a 2D 'corona' for filtering in the Foutier domain or in direct space.
 */
//-----------------------------------------------------------------------------

void draw_doughnut (double **galette, int size_h, int size_v, int h_centre, int v_centre, double freq_min, double marge_int, double freq_max, double marge_ext)
{

    { // mise initiale à 0 du tableau
        for (int iv = 0 ; iv<size_v ; iv++)
        for (int ih = 0 ; ih<size_h ; ih++)
            galette[iv][ih] = 0.0;	
    }
    
    const double PI = 3.1415926535;
    // defines galette at "1" between two concentric circles and "0" elsewhere
    double ra = freq_min;
    double rc = freq_max;
    // mask limits are smooth: go from 0 to 1 between ra and rb, then from 1 to 0 between rc and rd
    double rb = freq_min + marge_int;
    double rd = freq_max + marge_ext;

    double ra2 = ra * ra; 	// square these radii, for speed
    double rb2 = rb * rb;
    double rc2 = rc * rc;
    double rd2 = rd * rd;

    int h_min = (int) (h_centre - (freq_max + marge_ext));	// shrink bounds, for speed
    int h_max = (int) (h_centre + (freq_max + marge_ext));
    int v_min = (int) (v_centre - (freq_max + marge_ext));
    int v_max = (int) (v_centre + (freq_max + marge_ext));

    if (h_min < 0) h_min=0; 	// securities. Sould be shorter syntax such as 'cap((i_min,i_max,j_min,j_max), domainshape)'
    if (h_max < 0) h_max=0;
    if (v_min < 0) v_min=0;
    if (v_max < 0) v_max=0;
    if (h_min > size_h) h_min=size_h;
    if (h_max > size_h) h_max=size_h;
    if (v_min > size_v) v_min=size_v;
    if (v_max > size_v) v_max=size_v;

    for (int v = v_min ; v<v_max ; v++)
    {
        int dv= v - v_centre; 		// position center in v
        for (int h = h_min; h<h_max ; h++)
        {
            int dh= h - h_centre;       // position center in h
            int r2 = (dh*dh + dv*dv);	// square of current radius
        
            if (r2 < ra2)   galette[v][h] = 0.;
            else if (r2 < rb2)		// from 0 to 1, smoothly: 'Hanning window' like
            {
                double dr = (sqrt(r2) - ra) / marge_int;
                galette[v][h] = 0.5 * (1. - cos(PI * dr));
            }
            else if (r2 < rc2)      galette[v][h] = 1.;
            else if (r2 < rd2)		// from 1 to 0, smoothly
            {
                double dr = (sqrt(r2) - rc)  / (float)(marge_ext);
                galette[v][h] = 0.5 * (1. + cos(PI * dr));
            }
            else galette[v][h] = 0.;
        }
    }
}



/* UTILITAIRES D.SERRE */

//-------------------------------------------------------------------------------------
//		retourne la valeur absolue d'un double; suite a des pbs de "ambiguous access to
//			overloaded function" lors de la compil quand on est passes de carbon a 
//			appli directement OS X
//-------------------------------------------------------------------------------------

double absolu(double nombre)
{	if( nombre >= 0)
{	return nombre;
}
else
{	return (-nombre);
}
}

//-------------------------------------------------------------------------------------
//		retourne la valeur absolue d'un int; suite a des pbs de "ambiguous access to
//			overloaded function" lors de la compil quand on est passes de carbon a 
//			appli directement OS X
//-------------------------------------------------------------------------------------

int absolu_int(int nombre)
{	if( nombre >= 0)
{	return nombre;
}
else
{	return (-nombre);
}
}

//------------------------------------------------------------------------------------
//		retourne l'arrondi d'un nombre
//		ATTENTION: floor et ceil retournent des double, pas des int !! 
//			(vu sur 3 pages web)
//------------------------------------------------------------------------------------

double arrondi(double nombre)
{	if( nombre >= 0)
{	return floor(nombre + 0.5);
}
else
{	return ceil(nombre - 0.5);
}
}

//------------------------------------------------------------------------------------
//		retourne l'arrondi entier d'un nombre
//------------------------------------------------------------------------------------

int arrondi_int(double nombre)
{	if( nombre >= 0)
{	return (int)(floor(nombre + 0.5));
}
else
{	return (int)(ceil(nombre - 0.5));
}
}

