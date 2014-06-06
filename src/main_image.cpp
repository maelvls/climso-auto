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

#define ARDUINO_DEV_LIST "/dev/ttyACM0 /dev/ttyACM1 /dev/ttyACM2 /dev/tty.usbmodemfa131 /dev/tty.usbmodemfd111"

#include <iostream>
#include <iomanip> // Pour cout << setprecision(4)
#include <csignal> // Pour éviter de ctrl+C sans supprimer les objets en mémoire
using namespace std;

#include "image.h"
#include "cmd_arduino.h"

void boucleDeGuidage(); // Prototype fonction
int main(int argc, char **argv) {
	boucleDeGuidage();
	return 0;
}


int main2(int argc, char **argv) {
	string empl = "images-de-correlation/test-correl-mv/\0";
	Image* obj = Image::depuisTiff(empl+"obj.tif");
	Image* ref = Image::tracerFormeSoleil(232);
	ref->versTiff(empl+"ref.tif");

	Image* obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB,NOYAU_LAPLACIEN_TAILLE);
	Image* ref_lapl = ref->convoluer(NOYAU_LAPLACIEN_TAB,NOYAU_LAPLACIEN_TAILLE);

	Image* correl = obj_lapl->correlation_rapide(*ref_lapl,0.9);
	correl->versTiff(empl+"correl.tif");
	double l_max, c_max;
	correl->maxParInterpolation(&l_max, &c_max);
	cout << "Max : (x=" << c_max << ",y=" << l_max << ")" <<endl;

	cout << "Hauteur relative : " << correl->calculerHauteurRelativeAutour(l_max,c_max) << endl;
	delete correl;
	delete obj; delete ref;
	delete obj_lapl; delete ref_lapl;
}



CSBIGCam* initialiserCamera() {
	CSBIGCam *cam = NULL;
	PAR_ERROR err;
	int num_essai = 0;
	do {
		if (num_essai > 0) {
			sleep(5);
			cout << "Essai de connexion à la caméra numéro " << num_essai << ":"<< endl;
		}
		if(cam != NULL)
			delete cam;
		cam = new CSBIGCam(DEV_USB); // Création du device USB
		if ((err = cam->GetError()) != CE_NO_ERROR) {
			cerr << "Erreur avec la camera lors de la création de l'objet caméra : " << cam->GetErrorString() << endl;
		}
		else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la caméra
			cerr << "Erreur avec la camera lors de l'établissement du lien: " << cam->GetErrorString() << endl;
		}
		else {
			// Ajustement du mode de lecture en binning 8x8 (cf "start exposure 2 parameters" dans la doc)
			// Pbms rencontrés : le readout-mode "RM_NXN" ne semblait pas fonctionner : il a fallu corriger
			// le mode numéro 10 dans csbigcam.cpp. Je suspecte que cette fonctionnalité n'a pas été implémentée
			// dans csbigcam.cpp.
			cam->SetReadoutMode(RM_3X3); // Binning 3x3 matériel
		}
		num_essai++;
	}
	while(err != CE_NO_ERROR);
	return cam;
}
int initialiserArduino() {
	int num_essai = 0;
	int fd_arduino;
	do {
		if (num_essai > 0) {
			sleep(5);
			cout << "Essai de connexion à l'arduino numéro " << num_essai << endl;
		}
		if((fd_arduino=arduinoInitialiserCom(ARDUINO_DEV_LIST)) == ARDUINO_ERR) {
			cerr << "Erreur de communication avec Arduino" << endl;
			char rep[300];
			arduinoRecevoirReponse(fd_arduino,rep);
			printf("%s",rep);
		}
		num_essai++;
	}
	while(fd_arduino == ARDUINO_ERR);
	return fd_arduino;
}


void signalHandler(int signum)
{
    cout << "Interruption " << signum << " recue\n";
    exit(signum);
}

void boucleDeGuidage() {
	// Emplacement des images debug
	string emplacement = "images-de-correlation/test-sbig/\0";

	signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);

	int fd_arduino = initialiserArduino();
	
	//
	// Création de l'image ayant la forme du soleil puis laplacien
	//
	Image *ref = Image::tracerFormeSoleil(226);
	Image *ref_lapl = ref->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);

	int larg_img_cam, haut_img_cam;
	double l_max_initial,c_max_initial;
	double l_max, c_max;

	//
	// Connexion à la caméra
	//
	CSBIGImg *obj_sbig = new CSBIGImg;
	CSBIGCam *cam = initialiserCamera();

	//
	// Prise de vue, corrélation et interpolation pour trouver le centre du Soleil
	//
	while (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
        cerr << "Erreur avec la camera lors de la capture d'une image : " << cam->GetErrorString() << endl;
        delete cam; cam = initialiserCamera();
    }
	obj_sbig->AutoBackgroundAndRange();
    Image *obj_no_bin = Image::depuisSBIGImg(*obj_sbig);
    //obj_no_bin->versTiff(emplacement+"obj_initial");
    //obj_no_bin = Image::depuisTiff(emplacement+"obj_initial.tif");
	Image *obj = obj_no_bin->reduire(2);
    Image *obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
    Image *correl = obj_lapl->correlation_rapide(*ref_lapl, 0.70);
	correl->maxParInterpolation(&l_max_initial, &c_max_initial);

#if DEBUG
    obj->versTiff(emplacement+"t0_obj.tif");
	correl->versTiff(emplacement+"t0_correl.tif");
	obj_lapl->versTiff(emplacement+"t0_obj_lapl.tif");
    ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
	ref->versTiff(emplacement+"t0_ref.tif");
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
		while (cam->GrabImage(obj_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
			cerr << "Erreur avec la camera lors de la capture d'une image : " << cam->GetErrorString() << endl;
	        delete cam; cam = initialiserCamera();
		}
		obj_sbig->AutoBackgroundAndRange();
		obj_no_bin = Image::depuisSBIGImg(*obj_sbig);
	    //obj_no_bin = Image::depuisTiff(emplacement+"obj_initial.tif");
		obj = obj_no_bin->reduire(2); // Binning 2x2 logiciel
	    obj_lapl = obj->convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
		correl = obj_lapl->correlation_rapide(*ref_lapl, 0.70);
		correl->maxParInterpolation(&l_max, &c_max);
		double ratio = correl->calculerHauteurRelativeAutour(l_max,c_max);


#if DEBUG
		obj->versTiff(emplacement+"t_obj.tif");
		correl->versTiff(emplacement+"t_correl.tif");
		obj_lapl->versTiff(emplacement+"t_obj_lapl.tif");
#endif

		delete obj_sbig; 	// On devrait éviter de créer autant d'objet à chaque itération
		delete obj;			// Mais bon, re-coder tout est pénible
		delete correl;
		delete obj_lapl;
		delete obj_no_bin;
		//
		// Calcul du décalage x,y entre la position initiale
		//
		double l_decal = l_max - l_max_initial;
		double c_decal = c_max - c_max_initial;


		cout << "La prise de vue indique le Soleil à la position (x= "
				<< c_max << ", y=" <<l_max<<")" << endl;
		cout << "Le décalage avec l'image d'origine est de (x= "
				<< c_decal << ", y=" <<l_decal<<")" << endl;
		cout << "Ratio moyenne_autour_pix/valeur_pix : " << ratio << endl;


		while(arduinoEnvoyerCmd((l_decal<0)?PIN_SUD:PIN_NORD,
				((l_decal<0)?l_decal*(-1):l_decal)*IMPULSION_PIXEL_V,
				fd_arduino) == ARDUINO_ERR) {
			cerr << "Erreur de communication avec Arduino" << endl;
			fd_arduino = initialiserArduino();
		}
		while(arduinoEnvoyerCmd((c_decal<0)?PIN_OUEST:PIN_EST,
				((c_decal<0)?c_decal*(-1):c_decal)*IMPULSION_PIXEL_H,
				fd_arduino) == ARDUINO_ERR) {
			cerr << "Erreur de communication avec Arduino" << endl;
			fd_arduino = initialiserArduino();
		}
	} while(true);

	// Extinction du lien avec la caméra
	if (cam->CloseDevice() != CE_NO_ERROR) {
        cerr << "Erreur avec la camera : " << cam->GetErrorString() << endl;
        exit(1);
    }
}
