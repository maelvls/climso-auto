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

string emplacement = "";

Guidage::Guidage() {
	QObject::connect(&timerCorrection,SIGNAL(timeout()),this,SLOT(guidageSuivant()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(verifierLesConnexions()));
	timerVerificationConnexions.setInterval(1000);
	timerVerificationConnexions.start(); // lancer les connexions au démarrage

	timerCorrection.setInterval(5000);
	consigne_c = consigne_l = 0;
	cam = NULL;
	img = NULL;
	img_sbig = NULL;
	arduino = NULL;
	ref_lapl = NULL;
	diametre = 200;
	fichier_arduino = "";
}

void Guidage::lancerGuidage(bool lancer) {
	if(not lancer) { // Cas où on stoppe le guidage
		timerCorrection.stop();
		//emit message("Guidage arrete");
		return;
	} else {
		if(timerCorrection.isActive()) { // Si guidage déjà en cours
			timerCorrection.stop();
		}
		guidageInitial();
	}
}

void Guidage::lancerConnexions(bool lancer) {
	if(not lancer) { // Cas où on stoppe le guidage
		timerVerificationConnexions.stop();
	} else {
		timerVerificationConnexions.start();
	}
}

void Guidage::guidageInitial() {
	// traitement premiere image
	if(!arduinoConnecte() || !cameraConnectee()) {
		emit message("Verifiez les connexions avec la camera et l'arduino");
		return;
	}

	capturerImage(); // img est obtenu

	Image *obj_lapl = img->convoluerParDerivee();

	initialiserDiametre(diametre);

	Image *correl = obj_lapl->correlation_rapide_centree(*ref_lapl, 0.70);
	correl->maxParInterpolation(&consigne_l, &consigne_c);
	emit consigne(consigne_l,consigne_c);

	emit image(img);
	emit cercle(consigne_c/correl->getColonnes(),consigne_l/correl->getLignes(),((float)diametre)/correl->getColonnes());
	correl->versTiff(emplacement+"t0_correl.tif");

	double ratio = correl->calculerHauteurRelativeAutour(consigne_l,consigne_c);
	emit signalBruit(ratio);

#if DEBUG
	img->versTiff(emplacement+"t0_obj.tif");
	correl->versTiff(emplacement+"t0_correl.tif");
	obj_lapl->versTiff(emplacement+"t0_obj_lapl.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete correl;
	delete obj_lapl;

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
	capturerImage(); // obtention de img de l'image deja binee

	Image* obj_lapl = img->convoluerParDerivee();//convoluer(NOYAU_LAPLACIEN_TAB, NOYAU_LAPLACIEN_TAILLE);
	Image* correl = obj_lapl->correlation_rapide_centree(*ref_lapl, 0.70);
	double position_l, position_c;
	correl->maxParInterpolation(&position_l, &position_c);

	emit image(img);
	//emit cercle(position_c/correl->getColonnes(),position_l/correl->getLignes(),((float)diametre)/correl->getColonnes());
	emit cercle(consigne_c/correl->getColonnes(),consigne_l/correl->getLignes(),((float)diametre)/correl->getColonnes());

	double ratio = correl->calculerHauteurRelativeAutour(position_l,position_c);
	emit signalBruit(ratio);

#if DEBUG
	img->versTiff(emplacement+"t_obj.tif");
	correl->versTiff(emplacement+"t_correl.tif");
	obj_lapl->versTiff(emplacement+"t_obj_lapl.tif");
#endif

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
	if(nom.length()==0) {
		emit message("Le nom est vide");
		return;
	}
	else
		fichier_arduino = nom;
	if(arduino != NULL)
		delete arduino;
	arduino = new Arduino(nom.toStdString());
	if(arduino->getErreur()!=NO_ERR) {
		emit message(QString::fromStdString(arduino->getDerniereErreurMessage()));
	}
	else
		emit message("Arduino connecte (" + QString::fromStdString(arduino->getPath())+")");
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
    Image* img_temp = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig; // On supprime l'image CSBIGImg

    if(img) delete img; // On supprime la derniere image
    img = img_temp->reduire(2);
	delete img_temp;
    img_sbig = NULL;
}
void Guidage::demanderImage() {
	capturerImage();
    if(img)
    	emit image(img);
}

void Guidage::verifierLesConnexions() {
	if(!cameraConnectee()) {
		emit etatCamera(false);
		connecterCamera();
	}
	else {
		emit etatCamera(true);
	}
		if(!arduinoConnecte()) {
		emit etatArduino(false);
		connecterArduino(fichier_arduino);
	}
	else {
		emit etatArduino(true);
	}
	timerVerificationConnexions.start();
}

void Guidage::initialiserDiametre(int diametre) {
	if(ref_lapl) delete ref_lapl;
	Image *ref = Image::tracerFormeSoleil(diametre);
	ref_lapl = ref->convoluerParDerivee();

#if DEBUG
	ref->versTiff(emplacement+"t0_ref.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete ref;


	this->diametre = diametre;
	if(img) {
		emit image(img);
		emit cercle(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes());
	}
}

void Guidage::consigneModifier(int deltaLigne, int deltaColonne) {
	if(!img) return; // FIXME: verifier qu'il est possible de modifier la consigne
	// FIXME: verifier que la consigne modifiee est valide
	consigne_l = consigne_l + deltaLigne;
	consigne_c = consigne_c + deltaColonne;
	emit image(img);
	emit cercle(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes());
}
