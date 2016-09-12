//
//  camera.cpp
//  climso-auto
//
//  Created by Maël Valais on 11/09/2016.
//  Copyright © 2016 Maël Valais. All rights reserved.
//
#include "camera_sbig.h"

CameraSBIG::CameraSBIG() {
	cam = NULL;
	img = NULL;
}

CameraSBIG::~CameraSBIG() {
	deconnecter();
	//	if(cam) delete cam; cam=NULL;
	//	if(img) delete img; img=NULL;
}

/**
 * Déconnecte la caméra
 */
bool CameraSBIG::deconnecter() {
	if(cam) {
		cam->CloseDevice();
		delete cam; cam = NULL;
	}
	else {
		return false;
	}
	return true;
}

/**
 * Vérifie la bonne connexion avec la caméra
 * @return
 */
bool CameraSBIG::estConnectee() {
	return cam && cam->EstablishLink() == CE_NO_ERROR;
}


/**
 * Connecte la caméra
 */
bool CameraSBIG::connecter() {
	PAR_ERROR err;
	if(cam != NULL)
		delete cam;
	cam = new CSBIGCam(DEV_USB); // Creation du device USB
	if ((err = cam->GetError()) != CE_NO_ERROR) {
		return false;
	}
	else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
		return false;
	}
	else { // Pas d'erreurs, on met en binning 3x3
		cam->SetReadoutMode(RM_3X3);
		cam->SetExposureTime(DUREE_EXPOSITION * 0.001);
		cam->SetFastReadout(true);
		cam->SetABGState((ABG_STATE7)ABG_LOW7);
	}
	return true;
}

/**
 * Etape de capture d'une image et de binning 2x2
 * @return
 */
Image* CameraSBIG::capturer() {
	CSBIGImg* img_sbig = new CSBIGImg();
	if(cam->GrabImage(img_sbig, SBDF_LIGHT_ONLY) != CE_NO_ERROR) {
		return NULL;
	}
	Image* img_temp = Image::depuisSBIGImg(*img_sbig);
	delete img_sbig; // On supprime l'image CSBIGImg
	
	if(img) delete img; // On supprime la derniere image
	img = img_temp->reduire(2);
	delete img_temp;
	return img;
}

string CameraSBIG::derniereErreur() {
	return cam->GetErrorString();
}

