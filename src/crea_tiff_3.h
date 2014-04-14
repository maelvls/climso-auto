/*
------------------------------------------------------------------------
11-03-05 -------  Denis Serre
------------------------------------------------------------------------												
*/

void miroir_horizon(double **data,int nbpts_x,int nbpts_y);

void hexadec (int j, int *hexa);

void traitement_stream(int *buffer_int,int length);

void return_dim_tiff(int *nbpts,char str[256]);

void read_tiff_3(double **tableau_result,int nbpts_x,int nbpts_y,char str[256]);

void crea_tiff_4(int nbpts_x,int nbpts_y,int resolution,double val_sat_haute,double val_sat_basse,int valeur_imag_max,int valeur_imag_min,double **valeurs_points,char *nom_fichier_tiff,int nb_bits);

void crea_raw(int nbpts_x,int nbpts_y,double val_sat_haute,double val_sat_basse,long int valeur_imag_max,long int valeur_imag_min,double **valeurs_points,char *nom_fichier_raw,int nb_bits);

void crea_tiff_4_color(int nbpts_x,int nbpts_y,int resolution,double *val_sat_haute,double *val_sat_basse,int *valeur_imag_max,int *valeur_imag_min,double ***valeurs_points,char *nom_fichier_tiff,int nb_bits);

