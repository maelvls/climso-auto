/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 */

#include "guidage.h"
#include <QtCore/qmath.h>
#include <QtCore/QSettings>

string emplacement_capture = "";

Guidage::Guidage() {
	timerConnexionAuto.setInterval(PERIODE_ENTRE_CONNEXIONS);
	arduino = NULL;
	img = NULL;

	consigne_c = consigne_l = 0;
	bruitsignal = 1;

	// Paramètres de guidage
	orientationNordSud = ORIENTATION_NORD_SUD;
	orientationEstOuest = ORIENTATION_EST_OUEST;
	guidageEnMarche = false;

	QObject::connect(&timerConnexionAuto, SIGNAL(timeout()), this,
			SLOT(connexionAuto()));

	connexionAuto(); // lancer les connexions au démarrage
	timerConnexionAuto.start();

	// Test sauvegarde
	lireParametres();
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
	parametres.setValue("orientation-axe-nord-sud", orientationNordSud);
	parametres.setValue("orientation-axe-est-ouest", orientationEstOuest);
}

void Guidage::lireParametres() {
	QSettings parametres("irap", "climso-auto");
	consigne_c = parametres.value("derniere-consigne-x", 0).toDouble();
	consigne_l = parametres.value("derniere-consigne-y", 0).toDouble();
	orientationNordSud = parametres.value("orientation-axe-nord-sud", ORIENTATION_NORD_SUD).toInt();
	orientationEstOuest = parametres.value("orientation-axe-est-ouest", ORIENTATION_EST_OUEST).toInt();
}

void Guidage::connexionAuto() {
	if (!arduinoConnecte()) {
		emit etatArduino(ARDUINO_CONNEXION_AUTO_OFF);
		QStringList l = chercherFichiersArduino();
		if (l.length() > 0) {
			connecterArduino(l.first());
		} else {
			emit etatArduino(ARDUINO_FICHIER_INTROUV);
		}
		stopperGuidage(); // car il y a eu un arrêt de guidage
	} else {
		emit etatArduino(ARDUINO_CONNEXION_AUTO_ON);
	}
}

void Guidage::lancerGuidage() {
	if (img == NULL || position_c.isEmpty() || position_l.isEmpty()) {
		emit message(
				"Impossible de guider, la position n'a pas ete initialisee");
		guidageEnMarche = false;
	}
	else {
		guidageEnMarche = true;
	}
	if (consigne_c == 0 || consigne_l == 0) {
		initialiserConsigne();
	}
	emit etatGuidage(guidageEnMarche);
	tempsDepuisDernierGuidage.start();
}
void Guidage::stopperGuidage() {
	guidageEnMarche = false;
	emit etatGuidage(guidageEnMarche);
}

void Guidage::initialiserConsigne() {
	if (position_c.isEmpty() || position_l.isEmpty()) {
		emit message("Impossible d'initialiser la consigne, aucune position enregistree");
		return;
	}
	consigne_c = position_c.last();
	consigne_l = position_l.last();
	emit message(
			"Position initiale"
					" (x= " + QString::number(consigne_c) + ", y="
					+ QString::number(consigne_l) + ")");
}

void Guidage::guider() {
	if (!arduinoConnecte()) {
		emit message("Verifiez les connexions avec l'arduino");
		stopperGuidage();
		return;
	}
	if (position_c.isEmpty() || position_l.isEmpty()) {
		emit message("Aucune position courante");
		return;
	}
	// Calcul de la moyenne de décalage
	double moyenne_l = 0, moyenne_c = 0;
	int i;
	for (i = 0; position_l.length() > 0; i++) {
		moyenne_l += position_l.last();
		moyenne_c += position_c.last();
		position_l.removeLast();
		position_c.removeLast();
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
		emit message("Le guidage ne semble pas répondre");
		stopperGuidage();
	}

	if(decalage.length() > 100) {
		decalage.removeFirst();
	}

	emit message(
			"(x= " + QString::number(position_c.last()) + ", y="
					+ QString::number(position_l.last()) + ") "
							", (dx= " + QString::number(l_decal) + ", dy="
					+ QString::number(c_decal) + ")");
	int l_decal_duree = qAbs(l_decal * IMPULSION_PIXEL_V);
	int c_decal_duree = qAbs(c_decal * IMPULSION_PIXEL_H);

	if (l_decal_duree > tempsDepuisDernierGuidage.elapsed()-100) // La duree entre deux corrections ne doit pas depasser le timer
		l_decal_duree = tempsDepuisDernierGuidage.elapsed()-100;
	if (c_decal_duree > tempsDepuisDernierGuidage.elapsed()-100)
		c_decal_duree = tempsDepuisDernierGuidage.elapsed()-100;
	tempsDepuisDernierGuidage.restart();
	envoyerCmd((l_decal * ORIENTATION_NORD_SUD < 0) ? PIN_SUD : PIN_NORD,
			l_decal_duree);
	envoyerCmd((c_decal * ORIENTATION_EST_OUEST < 0) ? PIN_OUEST : PIN_EST,
			c_decal_duree);
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
		emit message(
				QString::fromStdString(arduino->getDerniereErreurMessage()));
	} else
		emit message(
				"Arduino connecte ("
						+ QString::fromStdString(arduino->getPath()) + ")");
}
void Guidage::deconnecterArduino() {
	if (arduino != NULL) {
		emit message(
				"Le fichier " + QString::fromStdString(arduino->getPath())
						+ " a ete ferme");
		delete arduino;
		arduino = NULL;
	} else
		emit message("Aucun fichier n'etait ouvert");
}

bool Guidage::arduinoConnecte() {
	return arduino && arduino->getErreur() == NO_ERR;
}

void Guidage::envoyerCmd(int pin, int duree) {
	if (arduino && arduino->getErreur() == NO_ERR) {
		arduino->EnvoyerCmd(pin, duree);
		emit message(
				"Envoi impulsion pin " + QString::number(pin) + " et duree "
						+ QString::number(duree));
	} else if (arduino) {
		emit message(
				"Arduino non connecte : "
						+ QString::fromStdString(
								arduino->getDerniereErreurMessage()));
	} else
		emit message("Aucun arduino connecte");
}

void Guidage::traiterResultatsCapture(Image* img, double l, double c,
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
	}
	afficherImageSoleilEtReperes();

	// Si l'historique des positions contient assez de poitions, on envoie le guidage
	if(guidageEnMarche && position_l.length() > ECHANTILLONS_PAR_GUIDAGE) {
		guider();
	}
}

void Guidage::modifierConsigne(int deltaLigne, int deltaColonne,
		int modeVitesse) {
	if (consigne_l == 0 || consigne_c == 0 || img == NULL) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l
			+ deltaLigne
					* ((modeVitesse == VITESSE_LENTE) ?
					INCREMENT_LENT :
														INCREMENT_RAPIDE);
	consigne_c = consigne_c
			+ deltaColonne
					* ((modeVitesse == VITESSE_LENTE) ?
					INCREMENT_LENT :
														INCREMENT_RAPIDE);
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
	if (bruitsignal < SEUIL_BRUIT_SIGNAL) {
		emit repereSoleil(position_c.last() / img->getColonnes(),
				position_l.last() / img->getLignes(),
				((float) diametre) / img->getColonnes(), POSITION_OK);
	} else {
		emit repereSoleil(position_c.last()/img->getColonnes(),position_l.last()/img->getLignes(),((float)diametre)/img->getColonnes(),POSITION_NOK);
	}
	if (not (consigne_l == 0 || consigne_c == 0)) {
		if (!decalage.isEmpty() && decalage.last() < SEUIL_DECALAGE_PIXELS) {
			emit repereConsigne(consigne_c / img->getColonnes(),consigne_l / img->getLignes(),((float) diametre) / img->getColonnes(), CONSIGNE_LOIN);
				} else if (true) {
					emit repereConsigne(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),CONSIGNE_LOIN);
		} else {
			emit repereConsigne(consigne_c/img->getColonnes(),consigne_l/img->getLignes(),((float)diametre)/img->getColonnes(),CONSIGNE_DIVERGE);
		}
	}
}

/*
 * Echantillon toutes les 1 secondes (on garde les positions)
 * Guidage tous les 3-4 échantillons récoltés
 * On envoie des impulsions et on garde l'historique des décalages ;
 * Si on constate (comment?) que le décalage augmente ou est égal,
 * 	on prévient et on arrête
 */
