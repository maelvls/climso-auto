/*  11/2/99 LK    entete pour interpol de Nevillle Aitken */
// ------- constantes ---------

// ---- protos --------
double it_pol_neville	 (int n, double *f, double alpha);
double it_pol_neville_s4 (double *f, double alpha);
double it_pol_neville_s3 (double *f, double alpha);
double it_pol_neville_s2 (double *f, double alpha);

double it_pol_neville2D (
	int n,				// taille du tableau source en x
	int m,				// taille du tableau source en y
	int gridX,			// taille de la zone prise pour interpolation en x
	int gridY,			// taille de la zone prise pour interpolation en y
	double **f,			// tableau source
	double x1,			// coordonnée x du point pour lequel on veut une val interpolée
	double y1);			// coordonnée y du point pour lequel on veut une val interpolée

double it_pol_neville2D_s4 (
	int n,				// taille du tableau source en x
	int m,				// taille du tableau source en y
	double **f,			// tableau source
	double x1,			// coordonnée x du point pour lequel on veut une val interpolée
	double y1);			// coordonnée y du point pour lequel on veut une val interpolée


double it_pol_neville2D_s3 (
	int n,				// taille du tableau source en x
	int m,				// taille du tableau source en y
	double **f,			// tableau source
	double x1,			// coordonnée x du point pour lequel on veut une val interpolée
	double y1);			// coordonnée y du point pour lequel on veut une val interpolée

double it_pol_neville2D_s2 (
	int n,				// taille du tableau source en x
	int m,				// taille du tableau source en y
	double **f,			// tableau source
	double x1,			// coordonnée x du point pour lequel on veut une val interpolée
	double y1);			// coordonnée y du point pour lequel on veut une val interpolée
