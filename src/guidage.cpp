/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 */

#include "guidage.h"
#include <QtCore/qmath.h>

string emplacement_capture = "";

Guidage::Guidage() {

	timerVerificationConnexions.setInterval(PERIODE_ENTRE_CONNEXIONS);
	timerCorrection.setInterval(PERIODE_ENTRE_GUIDAGES);
	consigne_c = consigne_l = 0;
	position_c = position_l = 0;
	arduino = NULL;
	img = NULL;
	bruitsignal = 1;
	decalage = 1000000;

	QObject::connect(&timerCorrection,SIGNAL(timeout()),this,SLOT(guidageAuto()));
	QObject::connect(&timerVerificationConnexions,SIGNAL(timeout()),this,SLOT(connexionAuto()));

	connexionAuto(); // lancer les connexions au démarrage
	timerVerificationConnexions.start();
}

Guidage::~Guidage() {
	deconnecterArduino();
//	if(arduino) delete arduino; arduino=NULL;
//	if(img) delete img; img=NULL;
	cout << "Arduino déconnecté" << endl;
}


void Guidage::connexionAuto() {
	if(!arduinoConnecte()) {
		emit etatArduino(ARDUINO_CONNEXION_AUTO_OFF);
		QStringList l = chercherFichiersArduino();
		if(l.length() > 0) {
			connecterArduino(l.first());
		}
		else {
			emit etatArduino(ARDUINO_FICHIER_INTROUV);
		}
		stopperGuidage(); // car il y a eu un arrêt de guidage
	} else {
		emit etatArduino(ARDUINO_CONNEXION_AUTO_ON);
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
		emit message("Impossible d'initialiser la consigne, aucune position enregistree");
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
	// Calcul de la moyenne de décalage

	double moyenne_l=0, moyenne_c=0; int i;
	for(i = 0; historique_l.length() > 0; i++) {
		moyenne_l += historique_l.first();
		moyenne_c += historique_c.first();
		historique_l.removeFirst();
		historique_c.removeFirst();
	}
	moyenne_l = moyenne_l/i;
	moyenne_c = moyenne_c/i;
	// Calcul du décalage x,y entre la position initiale
	double l_decal = moyenne_l - consigne_l;
	double c_decal = moyenne_c - consigne_c;
	// Calcul de la longueur en pixels du décalage
	decalage = qSqrt(l_decal*l_decal + c_decal*c_decal);

	emit message("(x= "+QString::number(position_c)+
				", y="+QString::number(position_l)+") "
				", (dx= "+QString::number(l_decal)+
				", dy="+QString::number(c_decal)+")");
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
	historique_l << l;
	historique_c << c;
	this->img = img;
	this->bruitsignal = bruitsignal;
	emit signalBruit(bruitsignal);
	emit imageSoleil(img);
	if(bruitsignal > SEUIL_BRUIT_SIGNAL) {
		stopperGuidage();
	}
	afficherImageSoleilEtReperes();
}

void Guidage::modifierConsigne(int deltaLigne, int deltaColonne,int modeVitesse) {
	if(consigne_l == 0 || consigne_c == 0 || img == NULL) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l + deltaLigne*((modeVitesse==VITESSE_LENTE)?INCREMENT_LENT:INCREMENT_RAPIDE);
	consigne_c = consigne_c + deltaColonne*((modeVitesse==VITESSE_LENTE)?INCREMENT_LENT:INCREMENT_RAPIDE);
	afficherImageSoleilEtReperes();
}

QStringList Guidage::chercherFichiersArduino() {
	// FIXME: il faut pouvoir distinguer le cas de fichier inexistant et le cas
	// du fichier dont les droits de lecture sont insuffisants !
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
void Guidage::afficherImageSoleilEtReperes() {
	emit imageSoleil(img);
	if(bruitsignal < SEUIL_BRUIT_SIGNAL) {
		emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes(),POSITION_OK);
	} else {
		emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes(),POSITION_NOK);
	}
	if(not(consigne_l == 0 || consigne_c == 0)) {
		if(decalage < SEUIL_DECALAGE_PIXELS) {
			emit repereConsigne(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),CONSIGNE_LOIN);
		} else if (true){
			emit repereConsigne(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),CONSIGNE_LOIN);
		} else {
			emit repereConsigne(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),CONSIGNE_DIVERGE);
		}
	}
}

/*
 * Echantillon toutes les 1 secondes (on garde les positions)
 * Guidage toutes les 5 secondes avec les échantillons récoltés
 */
