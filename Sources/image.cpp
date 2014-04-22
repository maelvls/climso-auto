//
//  image.cpp
//  climso-auto
//
//  Created by Maël Valais on 15/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

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
    //for (int i = 0; i < lignes; i++) {
    //    delete [] img[i];
    //}
    //delete [] img;
	//TODO: delete de Image à corriger
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
	@exception Not16bitsGrayScalePicture L'image n'est pas en échelles de gris sur 16 bits
	@exception ErrorOpeningPicture L'image ne peut être lue
*/
Image& Image::chargerTiff(string fichierEntree) {
    TIFF* tif = TIFFOpen(fichierEntree.c_str(), "r");
    if (tif == NULL) {
		throw ErrorOpeningPicture(fichierEntree);
	}
	uint32_t imagelength,imagewidth;
	tdata_t buffer;
	uint32_t ligne;
	uint32_t config;
	uint16_t samplePerPixel, bitsPerSample;
	
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	// Un pixel est constitué de samples. En intensités de gris, il
	// n'y a qu'un seul sample par pixel
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
	
	if(bitsPerSample != 16 || samplePerPixel != 1) {
		throw Not16bitsGrayScalePicture(bitsPerSample,samplePerPixel,fichierEntree);
	}

	// Préparation de la nouvelle image
	Image *out = new Image();
	out->lignes = imagelength;
	out->colonnes = imagewidth;
	out->img = new uint16_t*[imagelength];
	
	buffer = _TIFFmalloc(TIFFScanlineSize(tif));
			
	for (ligne = 0; ligne < imagelength; ligne++)
	{
		TIFFReadScanline(tif, buffer, ligne, 0); // 0 = le sample numéro 0
		out->img[ligne] = new uint16_t[imagewidth];
		for(int col=0; col<imagewidth; col++) {
			out->img[ligne][col] = ((uint16_t*)buffer)[col]; // XXX
		}
	}
	_TIFFfree(buffer);
	TIFFClose(tif);
	return *out;
}


int Image::ecrireTiff(string fichierSortie) {
	TIFF* out = TIFFOpen(fichierSortie.c_str(), "w");
	if (out == NULL) {
		throw ErrorOpeningPicture(fichierSortie);
	}
	TIFFSetDirectory(out, 0);

	TIFFSetField(out, TIFFTAG_IMAGELENGTH, lignes);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, colonnes);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, NOMBRE_SAMPLES_PAR_PIXEL);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, NOMBRE_BITS_PAR_SAMPLE);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // Orig de l'image
	//   Some other essential fields to set that you do not have to understand for now.
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION,"Image cree par la methode Image::ecrireTiff");
	
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
		memcpy(buf, img[l], linebytes);    // check the index here, and figure out why not using h*linebytes
		for(int i=0; i<3; i++)
			cout << *((uint16*)buf+i) << " ";
		if (TIFFWriteScanline(out, buf, l, 0) < 0)
			break;
	}
	(void) TIFFClose(out);
	if (buf)
		_TIFFfree(buf);
	
	return 0;
}
