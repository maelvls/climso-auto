#include <stdio.h>
#include <time.h>
#include "fcts_LK3.h"
#include "crea_tiff_3.h"
#include "convol.h"

typedef enum { false, true } bool;

/*
Ce code a été initialement écrit pour une convolution (LK_2008), ce qui explique certains noms de vcariables comme 'psf'. 
Il est utilisé ici pour une corrélation, ce qui est équivalent à une convolution au signe des axes près.
*/
//-----------------------------------------------------------------------------
//		convolution 'c' d'un objet 'o' par une reference 'r' :        c(a) = somme[ dx o(x) r(a-x) ]
//		params d'entree :	objet_src_initial,			pointeur vers le tableau  2D contenant l'objet 'o'
//							psf,						pointeur vers le tableau  2D contenant la reference 'r'
//							size_src_h, size_src_v		taille du tableau contenant l'objet 'o' == taille image convoluee 'c'
//							size_psf_h, size_psf_v		taille du tableau contenant la reference 'r'
//							seuil						valeur seuil des pixels dans 'r' en dessous de laquelle on court-circuite le calcul
//		param de sotrie :	convol_finale				pointeur vers le tableau  2D qui contiendra la convolution : 'c'
//-----------------------------------------------------------------------------
int	calc_convol	(double **objet_src_initial, double **psf, double **convol_finale, 
						int size_src_h, int size_src_v,
						int size_psf_h, int size_psf_v,
						double seuil)
{
	int size_psf_v_s2 = size_psf_v /2;
	int size_psf_h_s2 = size_psf_h /2;
	int size_calc_h = size_psf_h + size_src_h;	// calculera la conv dans un tableau plus grand que l'image
	int size_calc_v = size_psf_v + size_src_v;

	// tab de calcul de grande taille englobant l'image source et la PSF a cote.
	double**objet_src = (double**) alloc_mat_2D (size_calc_h, size_calc_v, sizeof(double));
	raz_mat_2D (objet_src, size_calc_h, size_calc_v); // mise a zero initiale du tableau objet_src
	{
		// copier l'image source initiale centree dans le tab de calcul, il y aura du zero_padding autour
		for (int va = 0 ; va <size_src_v ; va++)
		for (int ha = 0 ; ha <size_src_h ; ha++) 
			objet_src [va + size_psf_v_s2][ha + size_psf_h_s2] = objet_src_initial [va][ha];
	}
	// matrice de grande taille pour héberger la convol, englobant l'image source et la PSF a cote. On n'en gardera pas les bords (correspondant à la PSF?)
	double**la_convol = (double**) alloc_mat_2D (size_calc_h, size_calc_v, sizeof(double));
	raz_mat_2D (la_convol, size_calc_h, size_calc_v);
	
	// faire un masque Booléen qui servira pour limiter le calcul aux seuls points dans 'r' qui sont de valeur > seuil
	bool **mask = (bool**) alloc_mat_2D (size_psf_h, size_psf_v, sizeof(bool));
	{
        int nb_pts_non_nuls = 0;    // nombre de pixels > seuil
		double integ_psf = 0;       // sera la somme de toutes les valeurs dans le tableau
		double integ_sup_seuil = 0; // sera la somme des seules valeurs supérieures au seuil
		for (int vx = 0 ; vx <size_psf_v ; vx++)
		for (int hx = 0 ; hx <size_psf_h ; hx++)
		{
			double val_psf = psf[vx][hx];
            if (val_psf<0) val_psf = -val_psf;   // équivalent à : val_psf = |val_psf|
			integ_psf += val_psf;   // pour connaitre la fraction qui sera négligée dans le calcul futur
			if (val_psf > seuil)    // en réalité cela teste |val_psf| > seuil ; les pixels négatifs de 'r' peuvent aussi compter
			{
				mask [vx][hx] = true;	// 'true' indique qu'il faudra sommer 'o'+'c' pour ce pixel de 'r'
				nb_pts_non_nuls++;
				integ_sup_seuil += val_psf;
			}
			else mask [vx][hx] = false;	// 'false' indique qu'il ne faudra pas calculer pour ce pixel de 'r'
		}
		printf ("seuil =  %4.2e ; nb. pixels > seuil = %4d total pixels= %d \n fraction de l'image négligée %4.2e ; nb de boucles = %4.2e \n", 
				seuil, nb_pts_non_nuls, size_psf_h * size_psf_v,
				(integ_psf - integ_sup_seuil) / integ_psf, 
				(float)size_src_h * (float)size_src_v * nb_pts_non_nuls);
	}

	{
		// calcul de la convolution
		double t_start = (double)(clock());
		for (int vx = 0 ; vx <size_psf_v ; vx++)
		for (int hx = 0 ; hx <size_psf_h ; hx++)
		{
			if (mask [vx][hx])
			{
				double pixel_psf = psf [vx][hx];
				for (int va = size_psf_v_s2 ; va <size_calc_v - size_psf_v_s2 -1; va++)	// 'va' parcourt ims srce et conv en vertical
				{
					double* ptr_src_v = objet_src [va - vx + size_psf_v_s2] - hx + size_psf_h_s2;
					double* ptr_conv_v = la_convol [va];
					for (int ha = size_psf_h_s2 ; ha <size_calc_h - size_psf_h_s2 -1; ha++)	// ha parcourt ims srce et conv en horizontal
						ptr_conv_v[ha] += pixel_psf * ptr_src_v[ha];
// la ligne ci-dessus fait pareil mais plus vite que:
//						la_convol [va][ha] += psf [vx][hx] * objet_src [va -vx +size_psf_v_s2][ha -hx +size_psf_h_s2];
				}
			}
		}
		printf ("temps calcul = %4.2f s \n",  (double)(clock() - t_start) /CLOCKS_PER_SEC);
	}
	
	{
		// copier la partie calculée de la convolution vers le tableau résultat
		for (int va = 0 ; va <size_src_v ; va++)
		for (int ha = 0 ; ha <size_src_h ; ha++)
			convol_finale [va][ha] = la_convol [va + size_psf_v_s2][ha + size_psf_h_s2];
	}
	
/* ----------  ecriture du masque pour controle et debug : image tiff des pixels (au dessus du seuil) pris en compte dans le calcul
    double**mask_real = (double**) alloc_mat_2D (size_psf_h, size_psf_v, sizeof(double));
    if (mask_real == 0) return -1;
    for (int vx = 0 ; vx <size_psf_v ; vx++) // cree un masque la ou la psf est non nulle
    for (int hx = 0 ; hx <size_psf_h ; hx++)
        if (mask [vx][hx] != 0) mask_real [vx][hx] = psf [vx][hx];
        else mask_real [vx][hx] = 0.0;

     crea_tiff_4 (size_psf_h, size_psf_v,// dimensions image
     300,								// resolution
     1.0,-1.0,65535,0,					// valeurs max et min en entree, valeurs max et min en sortie
     mask_real,"/Users/laurent/coro_pic/C1C2L1L2_guidage/progs_centrage/correl_images/mask.tiff", 16);
     free_mat_2D ( (void**)mask_real);
---------- */
    
	free_mat_2D ( (void**)objet_src);
	free_mat_2D ( (void**)la_convol);
	free_mat_2D ( (void**)mask);
	return 0;	
}
