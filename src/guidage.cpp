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
#define ORIENTATION_NORD_SUD 		-1 // 1 quand le nord correspond au nord, -1 sinon
#define ORIENTATION_EST_OUEST		-1 // 1 quand l'est correspond à l'est, -1 sinon
#define SEUIL_BRUIT_SIGNAL			0.50
#define PERIODE_ENTRE_GUIDAGES		5000 // en ms
#define PERIODE_ENTRE_CONNEXIONS	1000
// FIXME: si temps envoi arduino > timer, pbm (on limite ce temps à un peu au dessous de celui du timer)

#define FICHIER_ARDUINO_DEFAUT "/dev/ttyACM0"

string emplacement_capture = "";

Guidage::Guidage() {

	timerVerificationConnexions.setInterval(PERIODE_ENTRE_CONNEXIONS);
	timerCorrection.setInterval(PERIODE_ENTRE_GUIDAGES);
	consigne_c = consigne_l = 0;
	position_c = position_l = 0;
	arduino = NULL;
	fichier_arduino = FICHIER_ARDUINO_DEFAUT;
	img = NULL;

	QObject::connect(&timerCorrection,SIGNAL(timeout()),this,SLOT(guidageAuto()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(connexionAuto()));

	connexionAuto(); // lancer les connexions au démarrage
	timerVerificationConnexions.start();
}


void Guidage::connexionAuto() {
	if(!arduinoConnecte()) {
		emit etatArduino(false);
		connecterArduino(chercherFichiersArduino().first());
		stopperGuidage();
	} else {
		emit etatArduino(true);
	}
}


void Guidage::lancerGuidage() {
	if(img==NULL || position_c == 0 || position_l == 0) {
		emit message("Impossible de guider, la position n'a pas ete initialisee");
		return;
	}
	if(consigne_c==0 || consigne_l==0) {
		initialiserConsigne();
	}
	emit etatGuidage(true);
	timerCorrection.start();
}
void Guidage::stopperGuidage() {
	timerCorrection.stop();
	emit etatGuidage(false);
	return;
}

void Guidage::initialiserConsigne() {
	if(position_c == 0 || position_l == 0) {
		emit message("Impossible de guider, la position n'a pas ete initialisee");
		return;
	}
	consigne_c = position_c;
	consigne_l = position_l;
	emit message("Position initiale"
			" (x= "+QString::number(consigne_c)+
			", y="+QString::number(consigne_l)+")");
}

void Guidage::guidageAuto() {
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
	int l_decal_duree = qAbs(l_decal * IMPULSION_PIXEL_V);
	int c_decal_duree = qAbs(c_decal * IMPULSION_PIXEL_H);
	if(l_decal_duree > PERIODE_ENTRE_GUIDAGES-100) // La duree entre deux corrections ne doit pas depasser le timer
		l_decal_duree = PERIODE_ENTRE_GUIDAGES-100;
	if(c_decal_duree > PERIODE_ENTRE_GUIDAGES-100)
		c_decal_duree = PERIODE_ENTRE_GUIDAGES-100;
	envoyerCmd((l_decal*ORIENTATION_NORD_SUD<0)?PIN_SUD:PIN_NORD, l_decal_duree);
	envoyerCmd((c_decal*ORIENTATION_EST_OUEST<0)?PIN_OUEST:PIN_EST, c_decal_duree);
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

void Guidage::traiterResultatsCapture(Image* img, double l, double c, int diametre, double bruitsignal) {
	this->diametre = diametre;
	position_c = c;
	position_l = l;
	this->img = img;
	emit signalBruit(bruitsignal);
	emit imageSoleil(img);
	if(bruitsignal < SEUIL_BRUIT_SIGNAL) {
		emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes(),Qt::green);
	} else {
		emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes(),Qt::gray);
		stopperGuidage();
	}
	if(not(consigne_l == 0 || consigne_c == 0)) {
		emit repereSoleil(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),Qt::yellow);
	}
}

void Guidage::modifierConsigne(int deltaLigne, int deltaColonne) {
	if(consigne_l == 0 || consigne_c == 0 || img == NULL) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l + deltaLigne;
	consigne_c = consigne_c + deltaColonne;
	emit imageSoleil(img);
	emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes(),Qt::green);
	emit repereSoleil(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),Qt::yellow);
}

QStringList Guidage::chercherFichiersArduino() {
	QStringList filtre;
	filtre << "ttyACM*" << "tty.USB*" << "ttyUSB*" << "tty.usbmodem*";
	QDir dir("/dev");
	QFileInfoList fichiers = dir.entryInfoList(filtre,QDir::System);
	QStringList resultat;
	for(int i = 0; i < fichiers.length(); i++) {
		resultat.append(fichiers.at(i).absoluteFilePath());
	}
	return resultat;
}
