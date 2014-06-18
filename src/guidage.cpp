/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: admin
 */

#include "guidage.h"

#define IMPULSION_PIXEL_H	25 //ms
#define IMPULSION_PIXEL_V	25 //ms
#define PIN_NORD			12
#define	PIN_SUD				11
#define	PIN_EST				10
#define PIN_OUEST			9

string emplacement = "images-de-correlation/test-qt/";

Guidage::Guidage() {
	QObject::connect(&timerCorrection,SIGNAL(timeout()),this,SLOT(guidageSuivant()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(verifierLesConnexions()));
	timerVerificationConnexions.start(100);
	timerCorrection.setInterval(5000);
	consigne_c = consigne_l = 0;
	cam = NULL;
	img = NULL;
	img_sbig = NULL;
	arduino = NULL;
	ref_lapl = NULL;

}

void Guidage::lancerGuidage(bool lancer) {
	if(not lancer) { // Cas où on stoppe le guidage
		timerCorrection.stop();
		emit message("Timer de guidage arrete");
		return;
	}
	// traitement premiere image
	if(!arduinoConnecte() || !cameraConnectee()) {
		emit message("Verifiez les connexions avec la camera et l'arduino");
		return;
	}

	capturerImage(); // img est obtenu
	emit image(img);

	Image *obj = img->reduire(2);
	Image *obj_lapl = obj->convoluerParDerivee();

	initialiserDiametre(200);

	Image *correl = obj_lapl->correlation_rapide(*ref_lapl, 0.70);
	correl->maxParInterpolation(&consigne_l, &consigne_c);
	emit consigne(consigne_l,consigne_c);

	double ratio = correl->calculerHauteurRelativeAutour(consigne_l,consigne_c);
	emit signalBruit(ratio);

#if DEBUG
	obj->versTiff(emplacement+"obj_t0.tif");
	correl->versTiff(emplacement+"correl_t0.tif");
	obj_lapl->versTiff(emplacement+"obj_lapl_t0.tif");
	ref_lapl->versTiff(emplacement+"ref_lapl_t0.tif");
#endif

	delete correl;
	delete obj_lapl;
	delete obj;

	emit message("Position initiale"
			" (x= "+QString::number(consigne_c)+
			", y="+QString::number(consigne_l)+")");
	timerCorrection.start();

}
void Guidage::guidageSuivant() {
	if(!arduinoConnecte() || !cameraConnectee()) {
		emit message("Verifiez les connexions avec la camera et l'arduino");
		timerCorrection.stop();
		return;
	}
	// traitement image suivante
	capturerImage(); // obtention de img
	emit image(img);

	Image* obj = img->reduire(2); // Binning 2x2 logiciel
	Image* obj_lapl = obj->convoluerParDerivee();//convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
	Image* correl = obj_lapl->correlation_rapide(*ref_lapl, 0.70);
	double position_l, position_c;
	correl->maxParInterpolation(&position_l, &position_c);
	double ratio = correl->calculerHauteurRelativeAutour(position_l,position_c);
	emit signalBruit(ratio);

#if DEBUG
	obj->versTiff(emplacement+"t_obj.tif");
	correl->versTiff(emplacement+"t_correl.tif");
	obj_lapl->versTiff(emplacement+"t_obj_lapl.tif");
#endif

	delete obj;			// Mais bon, re-coder tout est pénible
	delete correl;
	delete obj_lapl;
	//
	// Calcul du décalage x,y entre la position initiale
	//
	double l_decal = position_l - consigne_l;
	double c_decal = position_c - consigne_c;

	emit message("Position"
			" (x= "+QString::number(position_c)+
			", y="+QString::number(position_l)+") "
			", decalage (x= "+QString::number(l_decal)+
			", y="+QString::number(c_decal)+")");



	envoyerCmd((l_decal<0)?PIN_SUD:PIN_NORD,
			((l_decal<0)?l_decal*(-1):l_decal)*IMPULSION_PIXEL_V);
	envoyerCmd((c_decal<0)?PIN_OUEST:PIN_EST,
			((c_decal<0)?c_decal*(-1):c_decal)*IMPULSION_PIXEL_H);

	timerCorrection.start();
}

void Guidage::connecterArduino(QString nom) {
	if(nom.length()==0)
		emit message("Le nom est vide. Essayez /dev/ttyACM0 par exemple");
	if(arduino != NULL)
		delete arduino;
	arduino = new Arduino(nom.toStdString());
	if(arduino->getErreur()!=NO_ERR) {
		emit message(QString::fromStdString(arduino->getDerniereErreurMessage()));
	}
	else
		emit message("L'arduino est connecte a travers le fichier " + QString::fromStdString(arduino->getPath()));
}
void Guidage::deconnecterArduino() {
	if(arduino != NULL) {
		emit message("Le fichier "+QString::fromStdString(arduino->getPath())+" a ete ferme");
		delete arduino;
		arduino = NULL;
    }
    else
    	emit message("Aucun fichier n'etait ouvert");
}

bool Guidage::arduinoConnecte() {
    return arduino && arduino->getErreur()==NO_ERR;
}

void Guidage::envoyerCmd(int pin,int duree) {
    if(arduino && arduino->getErreur() == NO_ERR) {
        arduino->EnvoyerCmd(pin,duree);
        emit message("Envoi impulsion pin "+QString::number(pin)+" et duree "+QString::number(duree));
    }
    else if(arduino){
    	emit message("Arduino non connecte : "+QString::fromStdString(arduino->getDerniereErreurMessage()));
    }
    else
    	emit message("Aucun arduino connecte");
}


void Guidage::connecterCamera() {
    PAR_ERROR err;
    if(cam != NULL)
        delete cam;
    cam = new CSBIGCam(DEV_USB); // Creation du device USB
    if ((err = cam->GetError()) != CE_NO_ERROR) {
        emit emit message("Erreur avec la camera lors de la creation de l'objet camera : "+QString::fromStdString(cam->GetErrorString()));
    }
    else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
        emit message("Erreur avec la camera lors de l'etablissement du lien: "+QString::fromStdString(cam->GetErrorString()));
    }
    else { // Pas d'erreurs, on met en binning 3x3
        cam->SetReadoutMode(RM_3X3);
        emit message("Camera connectee");
    }
}

void Guidage::deconnecterCamera() {
    if(cam) {
        cam->CloseDevice();
        emit message("Camera deconnectee");
        delete cam; cam = NULL;
    }
    else {
        emit message("Aucune camera n'est connectee");
    }
}

bool Guidage::cameraConnectee() {
    return cam && cam->EstablishLink() == CE_NO_ERROR;
}

void Guidage::capturerImage() {
    if (!cameraConnectee()) {
        emit message("La camera n'est pas connectee");
        return;
    }
    if(img_sbig == NULL) {
    	img_sbig = new CSBIGImg();
    }
    if(cam->GrabImage(img_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
        emit message("Impossible de lire capturer l'image : "+QString::fromStdString(cam->GetErrorString()));
        return;
    }
    if(img) delete img; // On supprime la derniere image
    img = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig; // On supprime l'image CSBIGImg
    img_sbig = NULL;
}
void Guidage::demanderImage() {
	capturerImage();
    emit image(img);
}

void Guidage::verifierLesConnexions() {
	emit etatCamera(cameraConnectee());
	emit etatArduino(arduinoConnecte());
	timerVerificationConnexions.start(1000);
}

void Guidage::initialiserDiametre(int diametre) {
	if(ref_lapl) delete ref_lapl;
	Image *ref = Image::tracerFormeSoleil(diametre);
	ref_lapl = ref->convoluerParDerivee();

#if DEBUG
	ref->versTiff(emplacement+"ref_t0.tif");
	ref_lapl->versTiff(emplacement+"ref_lapl_t0.tif");
#endif

	delete ref;

}

void Guidage::consigneLigne(double l) {
	consigne_l = l;
}

void Guidage::consigneColonne(double c) {
	consigne_c = c;
}
