/* 4 mars 2008LK    entete pour convol */

// ---- protos --------

#ifdef __cplusplus
extern "C" {
#endif

int	calc_convol	(double **image_src, double **psf, double **la_convol, 
						int nbpts_src_x, int nbpts_src_y,
						int nbpts_psf_x, int nbpts_psf_y,
						double seuil);

#ifdef __cplusplus
}
#endif