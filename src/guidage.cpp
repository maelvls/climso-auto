/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: admin
 */

#include "guidage.h"



Guidage::Guidage() {
	QObject::connect(&timer,SIGNAL(timeout()),this,SLOT(guider()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(verifierLesConnexions()));
	timerVerificationConnexions.start(500);
	consigne_x = consigne_y = 0;
	duree = 1000;
	cam = NULL;
	img = NULL;
	img_sbig = NULL;
	arduino = NULL;
}

void Guidage::lancerGuidage() {
	// traitement premiere image
	timer.start(duree);
}
void Guidage::guidageSuivant() {
	// traitement image suivante
	timer.start(duree);
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
    return arduino && arduino->verifierConnexion();
}

void Guidage::envoyerCmd(int pin,int duree) {
    if(arduino && arduino->getErreur() == NO_ERR) {
        arduino->EnvoyerCmd(pin,duree*1000);
        emit message("Envoi impulsion");
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
    return cam && cam->CheckLink();
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
    if(img) {
    	delete img;
    }
    img = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig;
    img_sbig = NULL;
    emit image(img);
}

void Guidage::verifierLesConnexions() {
	if(!cameraConnectee()) {
		emit message("Camera non ok");
	}
	else
		emit message("Camera ok");
	if(!arduinoConnecte()) {
		emit message("Arduino non ok");
	}
	else
		emit message("Arduino ok");
	timerVerificationConnexions.start(1000);
}
