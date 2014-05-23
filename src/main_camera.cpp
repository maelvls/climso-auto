//
//  main_camera.c
//  climso-auto
//
//  Created by Maël Valais on 20/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

#include "csbigcam.h"
#include "csbigimg.h"

#include "camera.cpp"
#include "crea_tiff_3.h"

#define LINE_LEN 80
/*
int main2(int argc, const char *argv[])
{
	CSBIGImg *img_sbig = NULL;
	img_sbig->OpenImage("/Users/mael65/prog/images-de-correlation/test-sbig/test.sbig");
	
	double** img_tab_double;
	int haut_tab_double, larg_tab_double;
	char out_fichier[] = "/Users/mael65/prog/images-de-correlation/test-sbig/out.tif\0";
	
	transformerEnTableauDeDouble(img_sbig, img_tab_double, &haut_tab_double, &larg_tab_double);
	crea_tiff_4(larg_tab_double, haut_tab_double, 300,
				65535, 0,
				65535, 0, img_tab_double,
				out_fichier, 16);
	
	delete img_sbig;
	return 0;
}
 */

int main(int argc, const char *argv[])
{
	CSBIGCam *pCam = (CSBIGCam *)0;
	CSBIGImg *pImg = (CSBIGImg *)0;
	CSBIGImg *pImg1a = (CSBIGImg *)0;
	CSBIGImg *pImg1b = (CSBIGImg *)0;
	PAR_ERROR err;
	SBIG_FILE_ERROR ferr;
	char s[LINE_LEN];
	string sPort1, sPort2;
	int height, width;
	
	do { // allow break out
		// Try to Establish a link to the first camera

		sPort1 = "USB";
		cout << "Creating the SBIGCam Object on " << sPort1 << "..." << endl;
		pCam = new CSBIGCam(DEV_USB);

		if ( (err = pCam->GetError()) != CE_NO_ERROR )
			break;
		cout << "Establishing a Link to the " << sPort1 << " Camera..." << endl;
		if ( (err = pCam->EstablishLink()) != CE_NO_ERROR )
			break;
		cout << "Link Established to Camera Type: " << pCam->GetCameraTypeString() << endl;
		if ( (err = pCam->GetFullFrame(width, height)) != CE_NO_ERROR )
			break;
		
		// Take a full frame image and save compressed
		cout << "Taking full-frame light image on " << sPort1 << "..." << endl;
		pImg = new CSBIGImg;
		if ( (err=pCam->GrabImage(pImg, SBDF_LIGHT_ONLY)) != CE_NO_ERROR )
			break;
		pImg->AutoBackgroundAndRange();
		cout << "Saving compressed full-frame image..." << endl;
		if ( (ferr = pImg->SaveImage("img1.sbig", SBIF_COMPRESSED)) != SBFE_NO_ERROR )
			break;

#if INCLUDE_FITSIO
		pImg->HorizontalFlip();
		pImg->VerticalFlip();
		cout << "Saving FITS full-frame image..." << endl;
		if ( (ferr = pImg->SaveImage("img1.fits", SBIF_FITS)) != SBFE_NO_ERROR )
			break;
#endif
		
		// Take a half frame image and save compressed
		pCam->SetSubFrame(width/4, height/4, width/2, height/2);
		cout << "Taking half-frame dark-subtracted image on " << sPort1 << "..." << endl;
		pImg1a = new CSBIGImg;
		if ( (err=pCam->GrabImage(pImg1a, SBDF_DARK_ALSO)) != CE_NO_ERROR )
			break;
		pImg1a->AutoBackgroundAndRange();
		cout << "Saving compressed image..." << endl;
		if ( (ferr = pImg1a->SaveImage("img1a.sbig", SBIF_COMPRESSED)) != SBFE_NO_ERROR )
			break;
#if INCLUDE_FITSIO
		cout << "Saving FITS half-frame dark-subtracted image..." << endl;
		if ( (ferr = pImg1a->SaveImage("img1a.fits", SBIF_FITS)) != SBFE_NO_ERROR )
			break;
#endif
		
		// Take a full frame low res image
		pCam->SetSubFrame(0, 0, 0, 0);				// Restore Full Frame
		pCam->SetReadoutMode(RM_3X3);				// Low Res mode
		cout << "Taking  dark-subtracted low-res image on " << sPort1 << "..." << endl;
		pImg1b = new CSBIGImg;
		if ( (err=pCam->GrabImage(pImg1b, SBDF_DARK_ALSO)) != CE_NO_ERROR )
			break;
		pImg1b->AutoBackgroundAndRange();
		cout << "Saving compressed image..." << endl;
		if ( (ferr = pImg1b->SaveImage("img1b.sbig", SBIF_COMPRESSED)) != SBFE_NO_ERROR )
			break;
#if INCLUDE_FITSIO
		cout << "Saving FITS half-frame dark-subtracted image..." << endl;
		if ( (ferr = pImg1b->SaveImage("img1b.fits", SBIF_FITS)) != SBFE_NO_ERROR )
			break;
#endif
				
		// shut down cameras
		cout << "Closing Devices..." << endl;
		if ( (err = pCam->CloseDevice()) != CE_NO_ERROR )
			break;

		cout << "Closing Drivers..." << endl;
		if ( (err = pCam->CloseDriver()) != CE_NO_ERROR )
			break;
	} while (0);
	if ( err != CE_NO_ERROR )
		cout << "Camera Error: " << pCam->GetErrorString(err) << endl;
	else if ( ferr != SBFE_NO_ERROR )
		cout << "File Error: " << ferr << endl;
	else
		cout << "SUCCESS" << endl;
	cout << "Hit any key to continue:";
	cin.getline(s, LINE_LEN);;
	
	delete pCam;
	delete pImg;
	delete pImg1a;
	return EXIT_SUCCESS;

}