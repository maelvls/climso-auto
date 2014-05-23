/*
------------------------------------------------------------------------
LK - groupees par DS puis LK  -----   mars 2004 - juin 2009 - avril 2014
------------------------------------------------------------------------												
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct point {int x; int y;} point;

void** alloc_mat_2D (int n_cols, int n_lignes, int el_size);

void free_mat_2D (void **mat_ptr);

void raz_mat_2D (double **data, int size_x, int size_y);
void bin_2x2 (double **data_brut, double **data, int size_x, int size_y);
void recopie (double **data_src, int size_src_i, int size_src_j, int org_i, int org_j, double **data_dest, int size_dest_i, int size_dest_j);
void calc_laplacien (double **data, double **laplacien, int size_x, int size_y);
void calc_abs_deriv (double **data, double **absderiv, int size_x, int size_y);

double calc_max_all (double **data, int size_x, int size_y);

double calc_arg_max_all (double **data, point* xymax, int size_h, int size_v);


double calc_min_all (double **data, int size_x, int size_y);

double calc_moyenne_all (double **data, int size_x, int size_y);

void add_val (double **data, double val, int size_x, int size_y);

double calc_max_all_1D(double *data, int size);

int calc_max_all_1D_pos(double *data, int size);

double calc_min_all_1D(double *data, int size);

int calc_min_all_1D_pos(double *data, int size);

double calc_max_crop (double **data,int min_x, int max_x, int min_y, int max_y);

double calc_min_crop (double **data,int min_x, int max_x, int min_y, int max_y);

void normalise	(double **image_src, double **image_dest, int size_src_h, int size_src_v, double norm);

void carre_tab_2D (double **psf, int size_psf_h, int size_psf_v);

void pow4_tab_2D (double **psf, int size_psf_h, int size_psf_v);
void draw_doughnut (double **couronne, int size_h, int size_v, int i_ctre, int j_ctre, double r_min, double flou_int, double r_max, double flou_ext);
/* UTILITAIRES D.SERRE */
double absolu(double nombre);

int absolu_int(int nombre);

double arrondi(double nombre);

int arrondi_int(double nombre);

#ifdef __cplusplus
}
#endif
