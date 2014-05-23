//
//  main_image.c
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef DEBUG
	#define DEBUG 0
#endif

#include <iostream>
#include <iomanip> // Pour cout << setprecision(4)
#include "image.h"
#include "cmd_arduino.h"
#define IMPULSION_PIXEL_H	250 //ms
#define IMPULSION_PIXEL_V	250 //ms
#define PIN_NORD			3
#define	PIN_SUD				4
#define	PIN_EST				5
#define PIN_OUEST			6

using namespace std;

// Emplacement des images
const string emplacement = "/Users/mael65/prog/images-de-correlation/test-sbig/";

int main2(int argc, char **argv) {
	double l, c;
	Image *ref = Image::dessinerMasqueDeSoleil(481);
    Image *obj = Image::depuisTiff(emplacement+"obj.tif");
	
    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
    Image *ref_lapl = ref->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
	
    Image *correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
	correl->maxParInterpolation(&l, &c);
	
#if DEBUG
	cout << fixed << showpoint << setprecision(2);
	cout << "(l="<< l <<",c="<< c << ")" << endl;
	
	correl->versTiff(emplacement+"correl.tif");
	obj_lapl->versTiff(emplacement+"obj_lapl.tif");
    ref_lapl->versTiff(emplacement+"ref_lapl.tif");
	ref->versTiff(emplacement+"ref.tif");
#endif

	return 0;
}


int main1(int argc, char **argv) {
	int arduino;
	if((arduino=arduinoInitialiserCom("/dev/tty.usbmodemfa131")) == -1) {
		cerr << "Erreur de communication avec Arduino" << endl;
		//exit(1);
	}
	
	/*
	 * Création de l'image ayant la forme du soleil puis laplacien
	 */
	Image *ref = Image::dessinerMasqueDeSoleil(201);
	Image *ref_lapl = ref->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);

	int larg_img_cam, haut_img_cam;
	double l_max_initial,c_max_initial;
	double l_max, c_max;

	/*
	 * Connexion à la caméra
	 */
	CSBIGImg *obj_sbig = new CSBIGImg;
	CSBIGCam *cam = new CSBIGCam(DEV_USB);
	if (cam->GetError() != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Connexion à la caméra
	if (cam->EstablishLink() != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Récupération des paramètres de taille de la configurationa actuelle de la caméra
	if (cam->GetFullFrame(larg_img_cam, haut_img_cam) != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Ajustement du mode de lecture en binning 8x8 (cf "start exposure 2 parameters" dans la doc)
	// Pbms rencontrés : le readout-mode "RM_NXN" ne semblait pas fonctionner : il a fallu corriger
	// le mode numéro 10 dans csbigcam.cpp. Je suspecte que cette fonctionnalité n'a pas été implémentée
	// dans csbigcam.cpp.
	cam->SetReadoutMode(RM_3X3);

	/*
	 * Prise de vue, corrélation et interpolation pour trouver le centre du Soleil
	 */
	if (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	obj_sbig->AutoBackgroundAndRange();
    Image *obj_no_bin = Image::depuisSBIGImg(*obj_sbig);
	Image *obj = new Image(*obj_no_bin);
    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
    Image *correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
	correl->maxParInterpolation(&l_max_initial, &c_max_initial);

#if DEBUG
    obj->versTiff(emplacement+"obj.tif");
	correl->versTiff(emplacement+"correl.tif");
	obj_lapl->versTiff(emplacement+"obj_lapl.tif");
    ref_lapl->versTiff(emplacement+"ref_lapl.tif");
	ref->versTiff(emplacement+"ref.tif");
#endif
    
    delete obj_sbig;
    delete obj;
	delete correl;
	delete obj_lapl;

	cout << fixed << showpoint << setprecision(2);
	cout << "La première prise de vue indique le Soleil à la position (x= "
			<<c_max_initial<< ", y=" <<l_max_initial<<")" << endl;

	
	do {
		sleep(1);
		obj_sbig = new CSBIGImg();
		if (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
            cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
            exit(1);
        }
		obj_sbig->AutoBackgroundAndRange();
	    Image *obj = Image::depuisSBIGImg(*obj_sbig);
	    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
	    correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
		correl->maxParInterpolation(&l_max, &c_max);
        
        
        delete obj_sbig;
        delete obj;
        delete correl;
        delete obj_lapl;
		/*
		 * Calcul du décalage x,y entre la position initiale
		 */
		double l_decal = l_max - l_max_initial;
		double c_decal = c_max - c_max_initial;
		

		cout << "La prise de vue indique le Soleil à la position (x= "
		<< c_max << ", y=" <<l_max<<")" << endl;
		cout << "Le décalage avec l'image d'origine est de (x= "
		<< c_decal << ", y=" <<l_decal<<")" << endl;


		if(arduinoEnvoyerCmd(((l_decal<0)?l_decal*(-1):l_decal)*IMPULSION_PIXEL_V,
							 (l_decal<0)?PIN_SUD:PIN_NORD, arduino) == ARDUINO_ERR) {
            cerr << "Erreur de communication avec Arduino" << endl;
            //exit(1);
        }
		if(arduinoEnvoyerCmd(((c_decal<0)?c_decal*(-1):c_decal)*IMPULSION_PIXEL_H,
							 (c_decal<0)?PIN_OUEST:PIN_EST, arduino) == ARDUINO_ERR) {
            cerr << "Erreur de communication avec Arduino" << endl;
            //exit(1);
        }
	} while(true);

	// Extinction du lien avec la caméra
	if (cam->CloseDevice() != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	return 0;
}

int main(int argc, char **argv) {
	Image* i = new Image(10000,10000);
	i->~Image();
	return 0;
}

