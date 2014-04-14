/*  LK 2014-03-20 ;  origine de l'algo de correlation L.Koechlin 9 mars 2008, initialement pour simuler des images de disques stellaires
Ce programme
 va servir à trouver la position précise du centre du disque solaire, pour le guidage du coronographe CLIMSO
 il
- fabrique une image de référence : un disque uniforme à bords flous, de même diamètre (en pixels) que le disque solaire
- lit une image du Soleil depuis un fichier au format tiff,
- calcule les Laplaciens de l'image du Soleil et de la référence
        On utilise le laplacien car celui d'un disque uniforme a la plupart des points à 0, ce qui va permettre d'accélérer la corrélation
- calcule la corrélation entre le Laplacien de l'image et celui de la référence
        l'algorithme calcule par sommes directes et non par FFT, mais court-ccircuite les points en dessous d'une valeur seuil (parcimonie) 
- trouve la position du max dans la corrélation, indiquant le décentrage relatif entre le disque solaire et la référence
- recopie un carré autour du pic en vue d'interpoler 
- interpole pour augmenter la précision de position du max
- tests la valeur absolue de la dérivée à la place du Laplacien


 A FAIRE 2014 03 21
 - calculer le rayon du Soleil en fonction de la date, pour y ajuster le rayon de la référence (faiblesse : il faut la bonne date) 
    ou  bien adoucir le disque de ref pour que ça marche toute l'année (plus robuste mais moins rapide à calculer car plus de points /= 0)
 
 A FAIRE POUR STAGE
 A TOULOUSE
 
 
 - définir un cahier des charges général du soft de guidage
 - faire le soft d'interface caméra ccd en entrée : image caméra -> tableau en RAM accessible depuis code C
 - faire le soft d'interfaçe ports USB en sortie : valeur en RAM -> commande une durée d'impulsion sur un circuit relai
 - Interface utilisateur : faire tout en C++ ou mettre une couche en Python ?
 - schéma de l'interface utilisateur
 - calcul de l'asservissement (commencer avec une commande proportionnelle à l'erreur, puis évoluer vers un filtre de Kalmann)
 AU PIC
 - tests sur le ciel
 - ajustage des actions à envoyer à la table : étalonner la durée des impulsions, pas forcément linéeaires, trouver les bons seuils
 - faire une documentation et un manuel d'utilisation
 - test "foolproof" du soft de guidage par divers utilisateurs non avertis
 
*/
#include <stdlib.h>
#include <stdio.h>
#include "convol.h"     // algo de correlation (similaire à une convolution au signe près)
#include "crea_tiff_3.h"// lecture écriture de fichiers images tiff
#include "fcts_LK3.h"   // untilitaires sur images 2D
#include "interpol.h"

// --------- CONSTANTES ---------	
const double PI = 3.1415926535;
const double arc_seconds = 180 * 3600 / PI;	// nombre de secondes d'arc dans un radian
const double seuil = 0.85; // Pour accélerer le calcul de la convolution on négligera les pixels de valeur < seuil. Ici: seuil relatif par rapport au max.

const double r_soleil = 100;    // rayon en pixels du disque uniforme de référence (à ajuster au vrai rayon de l'image du Soleil)
const double marge_disc = 2.5;  // nb de pixels de flou au bord du disque uniforme de référence. Ce flou donne une tolérance sur le rayon du Soleil. 
const int marge_pic = 20;           // nb de pixels à prendre en compte autour du pic dans la corrélation, pour interpoler sa position. 
const double pas_interp = 1/8.0;    // le pas d'interpolation dans un pixel (précision de la position du pic).
const double marge_interp = marge_pic/pas_interp; //nb total de pts qu'il y aura dans le tableau interpolé contenant le pic et sa marge

const char *POINT_SLASH = "/Users/mael65/prog/correl_images/"; // remplacer par un moyen de trouver le directory local
// ------------------------------

int main (int argc, char * const argv[])
{
      // lecture du fichier contenant l'image du Soleil dont on doit trouver la position du centre
    char nom_Fich_in[512]; // danger : pas de sécurité si les noms de fichiers (path compris) dépassent 511 caractères
    char nom_Fich_out[512];
    sprintf (nom_Fich_in, "%s%s", POINT_SLASH, "Soleil_3352x2532.tif"); // nom du fichier contenant l'image du Soleil
    
    int *caracs = (int*) malloc (3*sizeof(int));	// alloc mémoire pour tableaux de paramètres en lecture des fichiers images tiff
    return_dim_tiff (caracs, nom_Fich_in);          // lecture paramètres de l'image tiff
    int size_obj_h = caracs[0];		// nb de pixels par ligne 
    int size_obj_v = caracs[1];		// nb de pixels par colonne

    // alloc mémoire puis lecture vers RAM d'un tableau 2D pour l'image du Soleil 
    double **objet = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	
    read_tiff_3 (objet, size_obj_h, size_obj_v, nom_Fich_in);

    // passage en binning 2*2. 
    // plus tard ces binnings seront à supprimer car on lira directement des images binées depuis la caméra
    double **objet_2 = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));
    bin_2x2 (objet, objet_2, size_obj_h/2, size_obj_v/2);
    free_mat_2D ( (void**)objet);
    size_obj_h /=2;
    size_obj_v /=2;
    
    // passage en binning 4*4
    double **objet_4 = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	 
    bin_2x2 (objet_2, objet_4, size_obj_h/2, size_obj_v/2);
    free_mat_2D ( (void**)objet_2);
    size_obj_h /=2;
    size_obj_v /=2;

    // passage en binning 8*8
    double **objet_8 = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	 
    bin_2x2 (objet_4, objet_8, size_obj_h/2, size_obj_v/2);
    free_mat_2D ( (void**)objet_4);
    size_obj_h /=2;
    size_obj_v /=2;
    
/*-------affichage pour controle ----------*/
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "objet_bin8.tif"); 
    crea_tiff_4 (size_obj_h, size_obj_v,// dimensions image
                 300,							// resolution
                 65535.0, 0.0,65535,0,             // valeurs max et min en entree, valeurs max et min en sortie
                 objet_8, nom_Fich_out, 16);       // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
/*-----------------------------------------*/
    
    // printf ("calcul du Laplacien de l'objet, ");
    double **lapl_obj = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	// alloc mémoire 2D pour le laplacien de l'objet 
/**/ // calc_laplacien (objet_8, lapl_obj, size_obj_h, size_obj_v);
/**/ calc_abs_deriv (objet_8, lapl_obj, size_obj_h, size_obj_v);
      // mettre le laplacien de l'objet à une valeur moyenne nulle pour éviter les effets de bord lors du "zero padding" ultérieur
    double moyenne = calc_moyenne_all (lapl_obj, size_obj_h, size_obj_v);
    add_val (lapl_obj, - moyenne, size_obj_h, size_obj_v);
    
    /*-------affichage pour controle ----------*/
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "lapl_obj.tif"); 
    crea_tiff_4 (size_obj_h, size_obj_v,			// dimensions image
                 300,								// resolution
                 32000.0, 0.0 ,65535,0,		// valeurs max et min en entree, valeurs max et min en sortie
                 lapl_obj, nom_Fich_out, 16);       // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    /*-----------------------------------------*/
    
      // printf ("calcul de l'image de référence, ");
    int size_reference_h = 2*(r_soleil + 2*marge_disc);  // on prend un cadre pas trop grand, ajusté au disque pour gagner en temps de calcul
    int size_reference_v = 2*(r_soleil + 2*marge_disc);
    double **reference = (double**) alloc_mat_2D (size_reference_h ,size_reference_v, sizeof(double));	// alloc memoire 2D pour la reference
    draw_doughnut (reference, size_reference_h, size_reference_v, size_reference_h/2, size_reference_v/2, 0, 0, r_soleil, marge_disc);
    
    /*-------affichage pour controle ----------*/
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "reference_C.tif"); 
    crea_tiff_4 (size_reference_h, size_reference_v,// dimensions image
                 300,								// resolution
                 1.0,0.,65535,0,                    // valeurs max et min en entree, valeurs max et min en sortie
                 reference, nom_Fich_out, 16);      // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    /*-----------------------------------------*/
    
    printf ("calcul du Laplacien de la référence \r\n ");
    double **lapl_ref = (double **)alloc_mat_2D (size_reference_h, size_reference_v, sizeof(double));	// alloc mem 2D pour laplacien de la référence 
/**/   // calc_laplacien (reference, lapl_ref, size_reference_h, size_reference_v);
/**/ calc_abs_deriv (reference, lapl_ref, size_reference_h, size_reference_v);
    double max_ref = calc_max_all (lapl_ref, size_reference_h, size_reference_v);

    /*-------affichage pour controle ----------*/
    double min_ref = calc_min_all (lapl_ref, size_reference_h, size_reference_v);
    printf ("seuil_relatif %f  minimum dans l'image de référence = %f et son max = %f \r\n", seuil, min_ref, max_ref);
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "lapl_ref.tif"); 
    crea_tiff_4 (size_reference_h, size_reference_v,// dimensions image
                 300,								// resolution
                 10.0, -10.0,65535,0,               // valeurs max et min en entree, valeurs max et min en sortie
                 lapl_ref, nom_Fich_out, 16);       // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    /*-----------------------------------------*/
    
    printf ("calcul de la correlation:\r\n");
    double **correl = (double **)alloc_mat_2D (size_obj_h, size_obj_v, sizeof(double));	// alloc memoire 2D pour "correl"
    calc_convol	(lapl_obj, lapl_ref, correl, size_obj_h, size_obj_v, size_reference_h, size_reference_v, seuil * max_ref);
    point xymax; // structure (x,y) qui contiendra les coordonnées du maximum trouvé dans la corrélation
    double valmax = calc_arg_max_all (correl, &xymax, size_obj_h, size_obj_v);
    int x_du_max = xymax.x;
    int y_du_max = xymax.y;
    printf ("max = %e, position du max: x= %d, y= %d\n\r", valmax, x_du_max, y_du_max);
    normalise	(correl, correl, size_obj_h, size_obj_v, 32000.0); // la corrélation peut contenir des grandes valeurs > 2^16

    /*-------affichage pour controle ----------*/
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "correl.tif"); 
    crea_tiff_4 (size_obj_h, size_obj_v,			// dimensions image
                 300,								// resolution
                 32000.0,-32000.0,65535,0,			// valeurs max et min en entree, valeurs max et min en sortie
                 correl, nom_Fich_out, 16);         // tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    /*-----------------------------------------*/
    
    double **autour_du_pic_brut = (double **)alloc_mat_2D (marge_pic, marge_pic, sizeof(double));	// alloc memoire 2D pour "autour_du_pic"
    raz_mat_2D (autour_du_pic_brut, marge_pic, marge_pic);
    recopie (correl, size_obj_h, size_obj_v, x_du_max - marge_pic/2, y_du_max - marge_pic/2, autour_du_pic_brut, marge_pic, marge_pic);
    
    // ----  écriture du résultat intermédiaire pour contrôle et debug
    normalise	(autour_du_pic_brut, autour_du_pic_brut, marge_pic, marge_pic, 32000.0);
    sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "autour_pic_brut.tif"); 
    crea_tiff_4 (marge_pic, marge_pic,              // dimensions image
                 300,								// resolution
                 32000.0,-32000.0,65535,0,			// valeurs max et min en entree, valeurs max et min en sortie
                 autour_du_pic_brut, nom_Fich_out, 16);	// tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
    
    double **autour_du_pic_interp = (double **)alloc_mat_2D (marge_interp, marge_interp, sizeof(double));	// alloc pour pic interpolé
    raz_mat_2D (autour_du_pic_interp, marge_interp, marge_interp);
    {
        for (int ix=0; ix<marge_interp; ix++)
        for (int iy=0; iy<marge_interp; iy++) 
        {
            double x1 = ix * pas_interp;
            double y1 = iy * pas_interp;
            autour_du_pic_interp[ix][iy] = it_pol_neville2D_s4 (marge_pic, marge_pic, autour_du_pic_brut, x1, y1); // valeur interpolée
        }        
    }
    
    calc_arg_max_all (autour_du_pic_interp, &xymax, marge_interp, marge_interp); // xymax aura la position du max dans le tableau interpolé
    double x_du_max_interp = x_du_max - marge_pic/2 + xymax.x * pas_interp;
    double y_du_max_interp = y_du_max - marge_pic/2 + xymax.y * pas_interp;
    printf ("position du max interpolée : x= %3.3f, y= %3.3f \n\r", x_du_max_interp, y_du_max_interp);

   // ----  écriture du résultat intermédiaire pour contrôle et debug
    normalise	(autour_du_pic_interp, autour_du_pic_interp, marge_interp, marge_interp, 32000.0);
     sprintf (nom_Fich_out, "%s%s", POINT_SLASH, "autour_pic_interp.tif"); 
     crea_tiff_4 (marge_interp, marge_interp,	// dimensions image
     300,                                       // resolution
     32000.0,-32000.0,65535,0,                  // valeurs max et min en entree, valeurs max et min en sortie
     autour_du_pic_interp, nom_Fich_out, 16);	// tableau dest, nom de fichier, nb de bits par pixel (8 ou 16)
 
    free_mat_2D ( (void**)lapl_obj);
    free_mat_2D ( (void**)reference);
    free_mat_2D ( (void**)lapl_ref);
    free_mat_2D ( (void**)correl);
    
    return 0;
}
