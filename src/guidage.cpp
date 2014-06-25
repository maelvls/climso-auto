/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: admin
 */

#include "guidage.h"

#define IMPULSION_PIXEL_H	400 //ms
#define IMPULSION_PIXEL_V	800 //ms (2px pour 1sec)
#define PIN_NORD			12
#define	PIN_SUD				11
#define	PIN_EST				10
#define PIN_OUEST			9
#define ORIENTATION_NORD_SUD 	-1 // 1 quand le nord correspond au nord, -1 sinon
#define ORIENTATION_EST_OUEST	-1 // 1 quand l'est correspond à l'est, -1 sinon
#define SEUIL_BRUIT_SIGNAL		0.50

#define FICHIER_ARDUINO_DEFAUT "/dev/ttyACM0"

string emplacement_capture = "";

Guidage::Guidage() {

	timerVerificationConnexions.setInterval(1000);
	timerCorrection.setInterval(5000);
	consigne_c = consigne_l = 0;
	c_max = l_max = 0;
	position_c = position_l = 0;
	arduino = NULL;
	fichier_arduino = FICHIER_ARDUINO_DEFAUT;

	QObject::connect(&timerCorrection,SIGNAL(timeout()),this,SLOT(guidageSuivant()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(connexionAuto()));

	connexionAuto(); // lancer les connexions au démarrage
	timerVerificationConnexions.start();
}


void Guidage::connexionAuto() {
	if(!arduinoConnecte()) {
		emit etatArduino(false);
		connecterArduino(fichier_arduino);
	} else {
		emit etatArduino(true);
	}
}


void Guidage::lancerGuidage() {
	timerCorrection.start();
	emit etatGuidage(true);
	guidageInitial();
}
void Guidage::stopperGuidage() {
	timerCorrection.stop();
	emit etatGuidage(false);
	return;
}

void Guidage::guidageInitial() {
	if(!arduinoConnecte()) {
		emit message("Verifiez les connexions avec l'arduino");
		return;
	}
	consigne_c = position_c;
	consigne_l = position_l;
	emit message("Position initiale"
			" (x= "+QString::number(consigne_c)+
			", y="+QString::number(consigne_l)+")");
	timerCorrection.start();
}



void Guidage::guidageSuivant() {
	if(!arduinoConnecte()) {
		emit message("Verifiez les connexions avec l'arduino");
		timerCorrection.stop();
		return;
	}
	if(position_l == 0 || position_c == 0) {
		emit message("Aucune position courante");
		return;
	}
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

	envoyerCmd((l_decal*ORIENTATION_NORD_SUD<0)?PIN_SUD:PIN_NORD,
			((l_decal<0)?l_decal*(-1):l_decal)*IMPULSION_PIXEL_V);
	envoyerCmd((c_decal*ORIENTATION_EST_OUEST<0)?PIN_OUEST:PIN_EST,
			((c_decal<0)?c_decal*(-1):c_decal)*IMPULSION_PIXEL_H);
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

void Guidage::modifierPosition(double l, double c, int l_max, int c_max, int diametre) {
	position_c = c; position_l = l;
	this->c_max = c_max;
	this->l_max = l_max;
	this->diametre = diametre;
}

void Guidage::consigneModifier(int deltaLigne, int deltaColonne) {
	if(consigne_l == 0 || consigne_c == 0) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l + deltaLigne;
	consigne_c = consigne_c + deltaColonne;
	emit repereSoleil(consigne_c/c_max,consigne_l/l_max,((float)diametre)/c_max);
}
