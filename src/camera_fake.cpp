//
//  camera_fake.cpp
//  climso-auto
//
//  Created by Maël Valais on 11/09/2016.
//  Copyright © 2016 Maël Valais. All rights reserved.
//

#include "camera_fake.h" // Soit camera_sbig.cpp, soit camera_fake.cpp; sinon conflit de symboles
// #include "stdlib.h" // for getenv()

CameraFake::CameraFake() {
	const char* dir = getenv("FAKE_TIF");
	dir = dir ? dir : "fake.tif";
	cout << "Camera: using fake images from file '" << dir << "' (from environement variable FAKE_TIF)\n";
	img = Image::depuisTiff(dir);
}

CameraFake::~CameraFake() {
}

/**
 * Déconnecte la caméra
 */
bool CameraFake::deconnecter() {
	return true;
}

/**
 * Vérifie la bonne connexion avec la caméra
 * @return
 */
bool CameraFake::estConnectee() {
	return true;
}


/**
 * Connecte la caméra
 */
bool CameraFake::connecter() {
	return true;
}

/**
 * Etape de capture d'une image de taille 3500/6 sur 2500/6
 * @return
 */
Image* CameraFake::capturer() {
	return img;
}

string CameraFake::derniereErreur() {
	return "N/A";
}
