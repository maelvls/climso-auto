//
//  image.cpp
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//
//	Vocabulaire TIFF :
//	- Strip : un ensemble de lignes ; généralement nb(strips)=nb(lignes)
//	- Pixel : ensemble de Samples ; en nuances de gris, nb(Samples)=1
//	- Sample : sous-partie du pixel ; 3 par pixel pour une image en couleurs, 1 pour du gris
//
//

#define _DEBUG_

#include "image.h"

Image::Image() {
    lignes = 0;
    colonnes = 0;
    img = NULL;
}
Image::Image(int hauteur, int largeur) {
    lignes = hauteur;
    colonnes = largeur;
    img = new uint16_t*[lignes];
    for (int i=0; i < lignes; i++) {
        img[i] = new uint16_t[colonnes];
    }
}
Image::~Image() {
    for (int i = 0; i < lignes; i++) {
        free(img[i]);
    }
    free(img); // XXX: utiliser delete [] img; ?

}

/**
    Charge une image TIFF dans un objet Image
    @param fichierEntree Le fichier à charger
    @return NULL si erreur, la référence de Image sinon
	@author Nehad Hirmiz (http://stackoverflow.com/a/20170682)
		modifié par Mael Valais
	@note L'avertissement "TIFFReadDirectory: Warning, Unknown field with tag 50838 (0xc696) encountered"
			veut dire que quelques tags du fichier TIFF n'ont pas pu être interprétés, ce qui veut
			certainement dire que le fichier contient des metadata personnalisés non reconnus.
			L'erreur n'a aucune influence sur le traitement.
	@exception FormatPictureException L'image n'est pas en échelles de gris sur 16 ou 8 bits
	@exception OpeningPictureException L'image ne peut être lue
*/
Image& Image::chargerTiff(string fichierEntree) {
    TIFF* tif = TIFFOpen(fichierEntree.c_str(), "r");
    if (tif == NULL) {
		throw OpeningException(fichierEntree);
	}
	uint32_t imagelength,imagewidth;
	tdata_t buffer;
	uint32_t ligne;
	uint32_t config;
	uint16_t samplePerPixel, bitsPerSample, fillOrder;
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	// Un pixel est constitué de samples. En intensités de gris, il
	// n'y a qu'un seul sample par pixel
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
	//TIFFGetField(tif, TIFFTAG_FILLORDER, fillOrder); // Ordre des bits dans l'octet
	

	// Vérification qu'on est bien en 16 ou 8 bits et en nuances de gris
	if((bitsPerSample != 16 && bitsPerSample != 8)
			|| samplePerPixel != 1) {
		throw FormatException(bitsPerSample,samplePerPixel,fichierEntree);
	}

	// Préparation de la nouvelle image
	Image *out = new Image();
	out->lignes = imagelength;
	out->colonnes = imagewidth;
	out->img = new uint16_t*[imagelength];
	
	buffer = _TIFFmalloc(TIFFScanlineSize(tif));
			
	for (ligne = 0; ligne < imagelength; ligne++)
	{
		TIFFReadScanline(tif, buffer, ligne, 0);
		out->img[ligne] = new uint16_t[imagewidth];
		for(int col=0; col < imagewidth; col++) { // Copie de la ligne buf dans img[]
			if(bitsPerSample == 16) // XXX 16 -> 16bits va un peu baisser les intensités
				out->img[ligne][col] = ((uint16_t*)buffer)[col];
			else if (bitsPerSample == 8) // OK
				out->img[ligne][col] = ((uint8_t*)buffer)[col];
		}
	}
	_TIFFfree(buffer);
	TIFFClose(tif);
	return *out;
}


int Image::ecrireTiff(string fichierSortie) {
	TIFF* out = TIFFOpen(fichierSortie.c_str(), "w");
	if (out == NULL) {

		
		
		throw OpeningException(fichierSortie);
	}
	TIFFSetField(out, TIFFTAG_SUBFILETYPE,0); // Nécessaire pour etre lue
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, lignes);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, colonnes);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, NOMBRE_SAMPLES_PAR_PIXEL);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, NOMBRE_BITS_PAR_SAMPLE);
	//TIFFSetField(out, TIFFTAG_STRIPBYTECOUNTS);
	//TIFFSetField(out, TIFFTAG_MINSAMPLEVALUE,0);
	//TIFFSetField(out, TIFFTAG_MAXSAMPLEVALUE,255);
	//TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // Orig de l'image
	//   Some other essential fields to set that you do not have to understand for now.
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK); // Min Is Black
	TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION,"Image generee");
	
	tsize_t linebytes = colonnes * NOMBRE_SAMPLES_PAR_PIXEL * NOMBRE_BITS_PAR_SAMPLE/8; // length in memory of one row of pixel in the image.
	cout << "Nombre de bytes par ligne : " << linebytes << endl;
	unsigned char *buf = NULL; // buffer used to store the row of pixel information for writing to file
	// Allocating memory to store the pixels of current row
	if (TIFFScanlineSize(out) == linebytes)
		buf =(unsigned char *)_TIFFmalloc(linebytes);
	else
		buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(out));
	
	// We set the strip size of the file to be size of one row of pixels
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, colonnes*NOMBRE_SAMPLES_PAR_PIXEL));
	
	// Now writing image to the file one strip at a time
	for (uint32 l = 0; l < lignes; l++)
	{
		memcpy(buf, img[l], linebytes);

		#ifdef _DEBUG_ //-- AFFICHAGE POUR DEBUG
		for(int i=0; i<3; i++)
			cout << *((uint16*)buf+i) << " ";
		cout << endl;
		#endif // ------FIN AFFICHAGE POUR DEBUG


		if (TIFFWriteScanline(out, buf, l, 0) < 0)
			break;
	}
	(void) TIFFClose(out);
	if (buf)
		_TIFFfree(buf);
	
	return 0;
}

int Image::getColonnes() { return colonnes;}
int Image::getLignes() {return lignes; }
double Image::getPix(int l, int c) { return img[l][c];}
void Image::setPix(int l, int c, double val) { img[l][c]=val;}

/**
 * Copie une image src dans l'image receveuse avec le décalage x,y
 * à partir d'en haut à gauche
 * @param src
 * @param x
 * @param y
 */
void Image::copier(Image& src, int l_decal, int c_decal) {
	int l_deb = max(0,0+l_decal), c_deb = max(0, 0+c_decal);
	int l_fin = min(this->lignes, src.lignes+l_decal), c_fin = min(this->colonnes, src.colonnes+c_decal);
	for (int l = l_deb; l < l_fin; ++l) {
		for (int c = c_deb; c < c_fin; ++c) {
			setPix(l,c,getPix(l+l_decal,c+c_decal));
		}
	}
}
void Image::copier(Image& src) {
	copier(src,0,0);
}

void Image::init(int val) {
	for (int l = O; l < lignes; ++l) {
		for (int c = 0; c < colonnes; ++c) {
			setPix(l,c,0);
		}
	}
}

/**
	@note Ce code a été initialement écrit pour une convolution (LK_2008), ce qui explique certains noms de vcariables comme 'psf'.
	Il est utilisé ici pour une corrélation, ce qui est équivalent à une convolution au signe des axes près.
	convolution 'c' d'un objet 'o' par une reference 'r' :        c(a) = somme[ dx o(x) r(a-x) ]
	@param p L'image de référence (psf...)
	@param seuil Le seuil au dessous duquel on calcule pas la convolution

	@note On pourrait éventuellement calculer le seuil tel que 95% des points ne soient pas calculés
*/

Image& Image::convolution(Image& p, double seuil) {

// FIXME : IL FAUT mettre les pixels à 64 bits ou 32 bits
	int size_psf_v_s2 = size_psf_v /2;
	int size_psf_h_s2 = size_psf_h /2;
	int size_calc_h = size_psf_h + size_src_h;	// calculera la conv dans un tableau plus grand que l'image
	int size_calc_v = size_psf_v + size_src_v;

	Image out = new Image(this->getLignes()+p.getLignes(), this->getColonnes()+p.getColonnes());
	out.init(0);
	out.copier(*this, p.getLignes()/2, p.getColonnes()/2);

	// tab de calcul de grande taille englobant l'image source et la PSF a cote.
	double**objet_src = (double**) alloc_mat_2D (size_calc_h, size_calc_v, sizeof(double));
	raz_mat_2D (objet_src, size_calc_h, size_calc_v); // mise a zero initiale du tableau objet_src
	{
		// copier l'image source initiale centree dans le tab de calcul, il y aura du zero_padding autour
		for (int va = 0 ; va <size_src_v ; va++)
		for (int ha = 0 ; ha <size_src_h ; ha++)
			objet_src [va + size_psf_v_s2][ha + size_psf_h_s2] = objet_src_initial [va][ha];
	}



	// matrice de grande taille pour héberger la convol, englobant l'image source et la PSF a cote. On n'en gardera pas les bords
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
			double val_psf = ref[vx][hx];
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
				double pixel_psf = ref [vx][hx];
				for (int va = size_psf_v_s2 ; va <size_calc_v - size_psf_v_s2 -1; va++)	// 'va' parcourt ims srce et conv en vertical
				{
					double* ptr_src_v = objet_src [va - vx + size_psf_v_s2] - hx + size_psf_h_s2;
					double* ptr_conv_v = la_convol [va];
					for (int ha = size_psf_h_s2 ; ha <size_calc_h - size_psf_h_s2 -1; ha++)	// ha parcourt ims srce et conv en horizontal
						ptr_conv_v[ha] += pixel_psf * ptr_src_v[ha];
// la ligne ci-dessus fait pareil mais plus vite que: la_convol [va][ha] += pixel_psf * objet_src [va -vx +size_psf_v_s2][ha -hx +size_psf_h_s2];
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














