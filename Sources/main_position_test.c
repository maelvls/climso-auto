
#include <stdlib.h>
#include <stdio.h>
#include "convol.h"     // algo de correlation (similaire à une convolution au signe près)
#include "crea_tiff_3.h"// lecture écriture de fichiers images tiff
#include "fcts_LK3.h"   // untilitaires sur images 2D
#include "interpol.h"

int main (int argc, char * const argv[])
{
    int *caracs = (int*) malloc (3*sizeof(int));
	// Objet
	char nom_objet[] = "/Users/mael65/prog/images-de-correlation/test-correlation-lk/obj_lapl.tif";
	return_dim_tiff (caracs, nom_objet);
    int size_obj_h = caracs[0];
    int size_obj_v = caracs[1];
    double **objet = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));
    read_tiff_3 (objet, size_obj_h, size_obj_v, nom_objet);
	
	
	// Référence
	char nom_ref[] = "/Users/mael65/prog/images-de-correlation/test-correlation-lk/ref_lapl.tif";
	return_dim_tiff (caracs, nom_ref);
    int size_ref_h = caracs[0];
    int size_ref_v = caracs[1];
    double **ref = (double **)alloc_mat_2D (size_ref_h, size_ref_v, sizeof(double));
    read_tiff_3 (ref, size_ref_h, size_ref_v, nom_ref);
    
	
	double max = calc_max_all(ref, size_ref_h, size_ref_v);
	double min = calc_min_all(ref, size_ref_h, size_ref_v);

	
	
	// Corrélation
    double **correl = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	// alloc memoire 2D pour "correl"
    calc_convol	(objet, ref, correl, size_obj_h, size_obj_v, size_ref_h, size_ref_v,(max-min)*0.90);
	
	point xymax; // structure (x,y) qui contiendra les coordonnées du maximum trouvé dans la corrélation
    double valmax = calc_arg_max_all (correl, &xymax, size_obj_h, size_obj_v);
    printf ("max = %e, position du max: x= %d, y= %d\n\r", valmax, xymax.x, xymax.y);
    normalise	(correl, correl, size_obj_h, size_obj_v, 32000); // la corrélation peut contenir des grandes valeurs > 2^16
	
	
	
    /*-------affichage pour controle ----------*/
	char nom_correl[] = "/Users/mael65/prog/images-de-correlation/test-correlation-lk/correl_lk.tif";
    crea_tiff_4 (size_obj_h, size_obj_v,			// dimensions image
                 300,								// resolution
                 32000.0,0,65535,0,			// valeurs max et min en entree, valeurs max et min en sortie
                 correl, nom_correl, 16);         // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    /*-----------------------------------------*/

    free_mat_2D ( (void**)objet);
    free_mat_2D ( (void**)ref);
    free_mat_2D ( (void**)correl);

    return 0;
}
