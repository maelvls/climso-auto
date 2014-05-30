//
//  main_image.c
//  climso-auto
//
//  Created by Maël Valais on 21/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#define IMPULSION_PIXEL_H	25 //ms
#define IMPULSION_PIXEL_V	25 //ms
#define PIN_NORD			12
#define	PIN_SUD				11
#define	PIN_EST				10
#define PIN_OUEST			9



#ifdef __APPLE__
#define ARDUINO_DEVICE "/dev/tty.usbmodemfa131"
#else
#define ARDUINO_DEVICE "/dev/ttyACM0"
#endif

#include <iostream>
#include <iomanip> // Pour cout << setprecision(4)
#include <csignal> // Pour éviter de ctrl+C sans supprimer les objets en mémoire

#include "image.h"
#include "cmd_arduino.h"


using namespace std;

// Emplacement des images
const string emplacement = "/home/admin/images-de-correlation/test-sbig/";

void signalHandler(int signum)
{
    cout << "Interruption " << signum << ") recue\n";
    exit(signum);
}

int main(int argc, char **argv) {
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);

	int arduino;
	if((arduino=arduinoInitialiserCom(ARDUINO_DEVICE)) == ARDUINO_ERR) {
		cerr << "Erreur de communication avec Arduino" << endl;
		char rep[300];
		arduinoRecevoirReponse(arduino,rep);
		printf("%s",rep);
		//exit(1);
	}
	
	/*
	 * Création de l'image ayant la forme du soleil puis laplacien
	 */
	Image *ref = Image::dessinerMasqueDeSoleil(238);
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
        cerr << "Erreur avec la camera lors de la création de l'objet caméra : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Connexion à la caméra
	if (cam->EstablishLink() != CE_NO_ERROR) {
        cerr << "Erreur avec la camera lors de l'établissement du lien: " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Récupération des paramètres de taille de la configurationa actuelle de la caméra
	if (cam->GetFullFrame(larg_img_cam, haut_img_cam) != CE_NO_ERROR) {
        cerr << "Erreur avec la camera lors de l'obtention de la configuration de la caméra : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	// Ajustement du mode de lecture en binning 8x8 (cf "start exposure 2 parameters" dans la doc)
	// Pbms rencontrés : le readout-mode "RM_NXN" ne semblait pas fonctionner : il a fallu corriger
	// le mode numéro 10 dans csbigcam.cpp. Je suspecte que cette fonctionnalité n'a pas été implémentée
	// dans csbigcam.cpp.
	cam->SetReadoutMode(RM_3X3); // Binning 3x3 matériel

	/*
	 * Prise de vue, corrélation et interpolation pour trouver le centre du Soleil
	 */
	if (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
        cerr << "Erreur avec la camera lors de la capture d'une image : " << endl << cam->GetErrorString() << endl;
        exit(1);
    }
	obj_sbig->AutoBackgroundAndRange();
    Image *obj_no_bin = Image::depuisSBIGImg(*obj_sbig);
	Image *obj = obj_no_bin->reduire(2);
    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
    Image *correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
	correl->maxParInterpolation(&l_max_initial, &c_max_initial);

#if DEBUG
    obj->versTiff(emplacement+"obj_t0.tif");
	correl->versTiff(emplacement+"correl_t0.tif");
	obj_lapl->versTiff(emplacement+"obj_lapl_t0.tif");
    ref_lapl->versTiff(emplacement+"ref_lapl_t0.tif");
	ref->versTiff(emplacement+"ref_t0.tif");
#endif
    
    delete obj_sbig;
    delete obj;
	delete correl;
	delete obj_lapl;
	delete obj_no_bin;

	cout << fixed << showpoint << setprecision(2);
	cout << "La première prise de vue indique le Soleil à la position (x= "
			<<c_max_initial<< ", y=" <<l_max_initial<<")" << endl;

	
	do {
		sleep(5); // attendre N secondes
		obj_sbig = new CSBIGImg();
		if (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
	        cerr << "Erreur avec la camera lors de la capture d'une image : " << endl << cam->GetErrorString() << endl;
            exit(1);
        }
		obj_sbig->AutoBackgroundAndRange();
	    obj_no_bin = Image::depuisSBIGImg(*obj_sbig);
	    obj = obj_no_bin->reduire(2); // Binning 2x2 logiciel
	    obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
	    correl = obj_lapl->correlation_lk(*ref_lapl, 0.70);
		correl->maxParInterpolation(&l_max, &c_max);
        
#if DEBUG
		obj->versTiff(emplacement+"obj.tif");
		correl->versTiff(emplacement+"correl.tif");
		obj_lapl->versTiff(emplacement+"obj_lapl.tif");
#endif

        delete obj_sbig;
        delete obj;
        delete correl;
        delete obj_lapl;
        delete obj_no_bin;
		/*
		 * Calcul du décalage x,y entre la position initiale
		 */
		double l_decal = l_max - l_max_initial;
		double c_decal = c_max - c_max_initial;
		

		cout << "La prise de vue indique le Soleil à la position (x= "
		<< c_max << ", y=" <<l_max<<")" << endl;
		cout << "Le décalage avec l'image d'origine est de (x= "
		<< c_decal << ", y=" <<l_decal<<")" << endl;


		if(arduinoEnvoyerCmd((l_decal<0)?PIN_SUD:PIN_NORD,
							((l_decal<0)?l_decal*(-1):l_decal)*IMPULSION_PIXEL_V,
							arduino) == ARDUINO_ERR) {
            cerr << "Erreur de communication avec Arduino" << endl;
            //exit(1);
        }
		if(arduinoEnvoyerCmd((c_decal<0)?PIN_OUEST:PIN_EST,
							((c_decal<0)?c_decal*(-1):c_decal)*IMPULSION_PIXEL_H,
							arduino) == ARDUINO_ERR) {
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

int main_fuite_memoire(int argc, char **argv) {
	Image* i = new Image(10000,10000);
	i->~Image();
	return 0;
}

