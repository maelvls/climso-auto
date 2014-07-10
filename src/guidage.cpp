/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 *
 *	Classe de traitement (contrairement à fenetreprincipale.cpp qui gère l'interface).
 *	Cette classe gère :
 *		- la connexion et déconnexion à l'arduino,
 *		- la réception des résultats envoyés par la classe Capture (position et image),
 *		- l'envoi des impulsions à l'arduino à partir des positions à partir des résultats,
 *	Cette classe est aussi responsable de l'envoi à WidgetImage des informations concernant
 *	l'image à afficher.
 *
 * FONCTIONNEMENT DU GUIDAGE:
 * 		- Echantillon toutes les 1 secondes (on garde les positions)
 * 		- Guidage tous les 3-4 échantillons récoltés
 * 		- On envoie des impulsions et on garde l'historique des décalages ;
 * 			Si on constate (comment?) que le décalage augmente ou est égal,
 * 			on prévient et on arrête
 */

#include "guidage.h"
#include <QtCore/qmath.h>
#include <QtCore/QSettings>

string emplacement_capture = "";

Guidage::Guidage() {
	timerConnexionAuto.setInterval(PERIODE_ENTRE_CONNEXIONS);
	arduino = NULL;

	consigne_c = consigne_l = 0;
	bruitsignal = 1;

	// Paramètres de guidage
	qRegisterMetaType<EtatPosition>("EtatPosition");
	qRegisterMetaType<EtatConsigne>("EtatConsigne");
	qRegisterMetaType<EtatArduino>("EtatArduino");
	qRegisterMetaType<EtatGuidage>("EtatGuidage");
	etatGuidage = GUIDAGE_ARRET_NORMAL;
	etatArduino = ARDUINO_CONNEXION_OFF;
	etatConsigne = CONSIGNE_NON_INITIALISEE;
	etatPosition = POSITION_NON_INITIALISEE;

	QObject::connect(&timerConnexionAuto, SIGNAL(timeout()), this,SLOT(connexionAuto()));

	connexionAuto(); // lancer les connexions au démarrage
	timerConnexionAuto.start();

	// Test sauvegarde
	chargerParametres();
}

Guidage::~Guidage() {
	enregistrerParametres();
	deconnecterArduino();
//	if(arduino) delete arduino; arduino=NULL;
//	if(img) delete img; img=NULL;
	cout << "Arduino déconnecté" << endl;
	// FIXME: Quand on a fait le quit(), les events sont tous traités ?
}

void Guidage::enregistrerParametres() {
	QSettings parametres("irap", "climso-auto");
	parametres.setValue("derniere-consigne-x", consigne_c);
	parametres.setValue("derniere-consigne-y", consigne_l);
	parametres.setValue("orient-nord-sud-inversee", orientVertiInversee);
	parametres.setValue("orient-est-ouest-inversee", orientHorizInversee);
}

void Guidage::chargerParametres() {
	QSettings parametres("irap", "climso-auto");
	consigne_c = parametres.value("derniere-consigne-x", 0).toDouble();
	consigne_l = parametres.value("derniere-consigne-y", 0).toDouble();
	orientVertiInversee = parametres.value("orient-nord-sud-inversee", false).toBool();
	orientHorizInversee = parametres.value("orient-est-ouest-inversee", false).toBool();
}

void Guidage::connexionAuto() {
	arduinoConnecte();
	if (!arduinoConnecte()) {
		emit envoiEtatArduino(ARDUINO_CONNEXION_OFF);
		QStringList l = chercherFichiersArduino();
		if (l.length() > 0) {
			connecterArduino(l.first());
		} else {
			emit envoiEtatArduino(ARDUINO_FICHIER_INTROUV);
		}
		stopperGuidage(); // car il y a eu un arrêt de guidage
	} else {
		emit envoiEtatArduino(ARDUINO_CONNEXION_ON);
	}
}

void Guidage::lancerGuidage() {
	if (img.isNull() || position_c.isEmpty() || position_l.isEmpty()) {
		emit message(
				"Impossible de guider, aucune position précédente");
		etatGuidage = GUIDAGE_BESOIN_POSITION;
	}
	else {
		etatGuidage = GUIDAGE_MARCHE;
	}
	if (consigne_c == 0 || consigne_l == 0) {
		consigneReset();
	}
	emit envoiEtatGuidage(etatGuidage);
	tempsDepuisDernierGuidage.start();
}
void Guidage::stopperGuidage() {
	decalage.clear(); // On vide l'historique des derniers décalages
	emit envoiEtatGuidage(etatGuidage);
}

void Guidage::consigneReset() {
	if (position_c.isEmpty() || position_l.isEmpty()) {
		emit message("Impossible, aucune position précédente");
		return;
	}
	consigne_c = position_c.last();
	consigne_l = position_l.last();
	emit message("Position initiale"
				" (x= " + QString::number(consigne_c) + ", y="
				+ QString::number(consigne_l) + ")");
	etatConsigne = CONSIGNE_OK;
	afficherImageSoleilEtReperes();
}

void Guidage::guider() {
	if (!arduinoConnecte()) {
		emit message("Verifiez les connexions avec l'arduino");
		etatGuidage = GUIDAGE_ARRET_PANNE;
		stopperGuidage();
		return;
	}
	if (position_c.isEmpty() || position_l.isEmpty()) {
		emit message("Aucune position courante");
		etatGuidage = GUIDAGE_BESOIN_POSITION;
		stopperGuidage();
		return;
	}
	// Calcul de la moyenne de décalage
	double moyenne_l = 0, moyenne_c = 0;
	int i;
	for (i = 0; i<position_l.length(); i++) {
		moyenne_l += position_l.last();
		moyenne_c += position_c.last();
	}
	moyenne_l = moyenne_l / i;
	moyenne_c = moyenne_c / i;
	// Calcul du décalage x,y entre la position initiale
	double l_decal = moyenne_l - consigne_l;
	double c_decal = moyenne_c - consigne_c;
	// Calcul de la longueur en pixels du décalage
	decalage << qSqrt(l_decal * l_decal + c_decal * c_decal);
	decalageTimestamp << QTime::currentTime();
	// Vérification de la divergence (les commandes n'ont pas d'effet/un effet contraire)
	double somme = 0;
	int indiceAncienDecalage = (decalage.length()>10)?decalage.length()-10:0;
	for(i = indiceAncienDecalage+1; i < decalage.length(); i++) {
		// Pour les 10 (ou moins) derniers décalages
		somme += decalage.at(i);
	}
	somme = somme / (decalage.length()-(indiceAncienDecalage+1)); // Car decalage.lenght() > 0
	if(somme > decalage.at(indiceAncienDecalage)) {
		emit message("Les commandes envoyées ne semblent pas avoir d'effet");
		emit envoiEtatGuidage(etatGuidage = GUIDAGE_ARRET_DIVERGE);
		stopperGuidage();
		afficherImageSoleilEtReperes();
	}

	if(decalage.length() > 100) {
		decalage.removeFirst();
	}


	// Il y a divergence si le dernier décalage est supérieur à la somme des


	emit message(
			"(x= " + QString::number(position_c.last()) + ", y="
					+ QString::number(position_l.last()) + ") "
							", (dx= " + QString::number(l_decal) + ", dy="
					+ QString::number(c_decal) + ")");
	position_c.clear();
	position_l.clear();


	int l_decal_duree = qAbs(l_decal * IMPULSION_PIXEL_V);
	int c_decal_duree = qAbs(c_decal * IMPULSION_PIXEL_H);

	// La duree entre deux corrections ne doit pas depasser la durée entre
	// deux séries d'échantillons (tempsDepuisDernierGuidage ici)
	if (l_decal_duree > tempsDepuisDernierGuidage.elapsed()-100)
		l_decal_duree = tempsDepuisDernierGuidage.elapsed()-100;
	if (c_decal_duree > tempsDepuisDernierGuidage.elapsed()-100)
		c_decal_duree = tempsDepuisDernierGuidage.elapsed()-100;
	tempsDepuisDernierGuidage.restart();

	// Envoi des commandes

	envoyerCmd((l_decal * (orientVertiInversee?-1:1)
			> 0 ? PIN_SUD	: PIN_NORD), l_decal_duree);
	envoyerCmd((c_decal * (orientHorizInversee?-1:1)
			> 0 ? PIN_OUEST	: PIN_EST), c_decal_duree);
}

void Guidage::connecterArduino(QString nom) {
	if (nom.length() == 0) {
		emit message("Le nom est vide");
		return;
	}
	if (arduino != NULL)
		delete arduino;
	arduino = new Arduino(nom.toStdString());
	if (arduino->getErreur() != NO_ERR) {
		if(arduino->getErreur() == ERR_OUVERTURE_FICHIER) {
			emit envoiEtatArduino(ARDUINO_FICHIER_INTROUV);
		} else {
			emit envoiEtatArduino(ARDUINO_CONNEXION_OFF);
		}
		emit message(QString::fromStdString(arduino->getDerniereErreurMessage()));
	} else {
		emit envoiEtatArduino(ARDUINO_CONNEXION_ON);
		emit message("Arduino connecte ("+ QString::fromStdString(arduino->getPath()) + ")");
	}
}

void Guidage::deconnecterArduino() {
	if (arduino != NULL) {
		emit message("Le fichier " + QString::fromStdString(arduino->getPath())+ " a ete ferme");
		delete arduino;
		arduino = NULL;
	} else
		emit message("Aucun fichier n'etait ouvert");
}

bool Guidage::arduinoConnecte() {
	return arduino && arduino->getErreur() == NO_ERR;
	// FIXME return arduino && arduino->verifierConnexion();
}

void Guidage::envoyerCmd(int pin, int duree) {
	if (arduinoConnecte()) {
		arduino->EnvoyerCmd(pin, duree);
		emit message("Envoi impulsion pin " + QString::number(pin) + " et duree "+ QString::number(duree));
	}
}

void Guidage::traiterResultatsCapture(QImage img, double l, double c,
		int diametre, double bruitsignal) {
	this->diametre = diametre;
	position_l << l;
	position_c << c;
	this->img = img;
	this->bruitsignal = bruitsignal;
	emit signalBruit(bruitsignal);
	emit imageSoleil(img);
	if (bruitsignal > SEUIL_BRUIT_SIGNAL) {
		stopperGuidage();
		etatPosition = POSITION_INCOHERANTE;
		etatGuidage = GUIDAGE_ARRET_BRUIT;
	}
	else {
		etatPosition = POSITION_COHERANTE;
	}
	afficherImageSoleilEtReperes();
	// Si l'historique des positions contient assez de poitions, on envoie le guidage
	if(etatGuidage == GUIDAGE_MARCHE && position_l.length() > ECHANTILLONS_PAR_GUIDAGE) {
		guider();
	}
}

void Guidage::modifierConsigne(int deltaLigne, int deltaColonne, bool decalageLent) {
	if (consigne_l == 0 || consigne_c == 0 || img.isNull()) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l+ deltaLigne* (decalageLent?INCREMENT_LENT:INCREMENT_RAPIDE);
	consigne_c = consigne_c+ deltaColonne* (decalageLent?INCREMENT_LENT:INCREMENT_RAPIDE);
	afficherImageSoleilEtReperes();
}

QStringList Guidage::chercherFichiersArduino() {
	// FIXME: il faut pouvoir distinguer le cas de fichier inexistant et le cas
	// du fichier dont les droits de lecture sont insuffisants !
	QStringList filtre;
	filtre << "ttyACM*" << "tty.USB*" << "ttyUSB*" << "tty.usbmodem*";
	QDir dir("/dev");
	QFileInfoList fichiers = dir.entryInfoList(filtre, QDir::System);
	QStringList resultat;
	for (int i = 0; i < fichiers.length(); i++) {
		resultat.append(fichiers.at(i).absoluteFilePath());
	}
	return resultat;
}
void Guidage::afficherImageSoleilEtReperes() {
	emit imageSoleil(img);
	emit repereSoleil(position_c.last()/img.width(),
			position_l.last()/img.height(),((float)diametre)/img.width(),etatPosition);
	emit repereConsigne(consigne_c/img.width(),
			consigne_l/img.height(),((float)diametre)/img.width(),etatConsigne);

}
