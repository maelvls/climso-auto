/*
The MIT License (MIT)

Copyright (c) 2015 Institut en Recherche en Astrophysique et en Planétologie, Toulouse

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*  11/2/99 LK    entete pour interpol de Nevillle Aitken */
// ------- constantes ---------

// ---- protos --------

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
