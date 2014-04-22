/*
	main.cpp - This file is a test of the CSBIGCam and CSBIGImage
			   classes.

	1. This software (c)2004 Santa Barbara Instrument Group.
	2. This free software is provided as an example of how 
	   to communicate with SBIG cameras.  It is provided AS-IS
	   without any guarantees by SBIG of suitability for a 
	   particular purpose and without any guarantee to be 
	   bug-free.  If you use it you agree to these terms and
	   agree to do so at your own risk.
    3. Any distribution of this source code to include these
	   terms.

	Revision History
	Date		Modification
	=========================================================
	1/26/04		Initial release
	11/9/04		Added compile time option for Ethernet Port camera 1
	1/17/05		Added taking and saving partial frame
				 on camera 1 and if compiled with FITSIO
				 saving FITS image

*/
#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

#include "csbigcam.h"
#include "csbigimg.h"

#define CAM1_ETHERNET	1						/* set to 1 to have first camera be Ethernet based so can run with EthSim */
#define CAM2_USB2			1						/* set to 1 to put 2nd camera on USB2 */
#define IP_ADDRESS		0xC0A80049	/* IP address of Ethernet Camera, 192.168.0.73 in this case */
#define LINE_LEN 			80

int main(/*int argc, char *argv[]*/)
{
	CSBIGCam *pCam = (CSBIGCam *)0;
	CSBIGCam *pCam2 = (CSBIGCam *)0;
	CSBIGImg *pImg = (CSBIGImg *)0;
	CSBIGImg *pImg1a = (CSBIGImg *)0;
	CSBIGImg *pImg1b = (CSBIGImg *)0;
	CSBIGImg *pImg2 = (CSBIGImg *)0;
	PAR_ERROR err;
	SBIG_FILE_ERROR ferr;
	char s[LINE_LEN];
	string sPort1, sPort2;
	int height, width;

	do { // allow break out
		// Try to Establish a link to the first camera
#if CAM1_ETHERNET
		OpenDeviceParams odp;
		odp.deviceType = DEV_ETH;
		odp.ipAddress = IP_ADDRESS;
		sPort1 = "Ethernet";
		cout << "Creating the SBIGCam Object on " << sPort1 << "..." << endl;
		pCam = new CSBIGCam(odp);
#else
		sPort1 = "USB";
		cout << "Creating the SBIGCam Object on " << sPort1 << "..." << endl;
		pCam = new CSBIGCam(DEV_USB);
#endif
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


		// try to establish link to second camera on LPT port
#if CAM2_USB2
		pCam2 = new CSBIGCam(DEV_USB2);
		sPort2 = "USB2";
#else
		pCam2 = new CSBIGCam(DEV_LPT1);
		sPort2 = "LPT1";
#endif
		cout << "Creating 2nd SBIGCam Object on " << sPort2 << "..." << endl;
		if ( (err = pCam2->GetError()) != CE_NO_ERROR )
			break;
		cout << "Establishing a Link to the " << sPort2 << " Camera..." << endl;
		if ( (err = pCam2->EstablishLink()) != CE_NO_ERROR )
			break;
		cout << "Link Established to Camera Type: " << pCam2->GetCameraTypeString() << endl;

		// take a full frame image and save uncompressed
		cout << "Taking light image on " << sPort2 << "..." << endl;
		pImg2 = new CSBIGImg;
		if ( (err=pCam2->GrabImage(pImg2, SBDF_LIGHT_ONLY)) != CE_NO_ERROR )
			break;
		pImg2->AutoBackgroundAndRange();
		cout << "Saving uncompressed image..." << endl;
		if ( (ferr = pImg2->SaveImage("img2.sbig", SBIF_UNCOMPRESSED)) != SBFE_NO_ERROR )
			break;

		// shut down cameras
		cout << "Closing Devices..." << endl;
		if ( (err = pCam->CloseDevice()) != CE_NO_ERROR )
			break;
		if ( (err = pCam2->CloseDevice()) != CE_NO_ERROR )
			break;
		cout << "Closing Drivers..." << endl;
		if ( (err = pCam->CloseDriver()) != CE_NO_ERROR )
			break;		
		if ( (err = pCam2->CloseDriver()) != CE_NO_ERROR )
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
	delete pCam2;
	delete pImg;
	delete pImg1a;
	delete pImg2;
	return EXIT_SUCCESS;
}
