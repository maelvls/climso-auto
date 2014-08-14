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
 * 		- Guidage tous les 3-4 positions calculées
 * 		- On envoie des impulsions et on garde l'historique des décalages ;
 * 			Si on constate (comment?) que le décalage augmente ou est égal,
 * 			on prévient et on arrête
 */

/**
 * TODO:
 * 		- implémenter diam_soleil
 * ?? 	- pourquoi il y a des blocages de 8sec pendant la prise de vue
 * OK 	- ajout fichier config seuilBruit
 * ?? 	- SegFault quand le soleil est trop loin
 */


#include "guidage.h"
#include <QtCore/qmath.h>
#include <QtCore/QSettings>

/**
 * Paramètres chargés pour le guidage (fichier ~/.config/irap/climso-auto.conf sous linux)
 * On peut modifier les paramètres par défaut en modifiant la valeur du
 * second paramètres de value(): value("le-nom-du-parametre",valeurParDéfaut)
 * Certains de ces paramètres sont aussi modifiables depuis le logiciel (menu > paramètres)
 */
void Guidage::chargerParametres() {
	QSettings parametres("irap", "climso-auto");

	consigne_l = parametres.value("derniere-consigne-y", 0).toDouble();
	consigne_c = parametres.value("derniere-consigne-x", 0.0).toDouble();
	orientVertiInversee = parametres.value("orient-nord-sud-inversee", true).toBool(); // 1 quand le nord correspond au nord, 0 sinon
	orientHorizInversee = parametres.value("orient-est-ouest-inversee", true).toBool(); // 1 quand l'est correspond à l'est, 0 sinon
	arretSiEloignement = parametres.value("arret-si-eloigne", false).toBool();
	gainHorizontal = parametres.value("gain-horizontal", 600).toInt();
	gainVertical = parametres.value("gain-vertical", 1000).toInt();
	dureeApresMauvaisSignalBruit = parametres.value("duree-attente-avant-arret", 2).toInt()*1000*60;
	seuilSignalSurBruit = parametres.value("seuil-signal-bruit",20).toDouble(); // Seuil Signal/bruit au dessous duquel le guidage ne peut être effectué (nuages..)
	if(consigne_l > 0) etatConsigne = CONSIGNE_LOIN;
}

void Guidage::enregistrerParametres() {
	QSettings parametres("irap", "climso-auto");
	parametres.setValue("derniere-consigne-x", consigne_c);
	parametres.setValue("derniere-consigne-y", consigne_l);
	parametres.setValue("orient-nord-sud-inversee", orientVertiInversee);
	parametres.setValue("orient-est-ouest-inversee", orientHorizInversee);
	parametres.setValue("arret-si-eloigne", arretSiEloignement);
	parametres.setValue("gain-horizontal", gainHorizontal);
	parametres.setValue("gain-vertical", gainVertical);
	parametres.setValue("duree-attente-avant-arret", dureeApresMauvaisSignalBruit/1000/60);
	parametres.setValue("seuil-signal-bruit",seuilSignalSurBruit);
}

Guidage::Guidage() {
	// Ces types doivent être enregistrés auprès de Qt pour les utiliser en signal/slot
	qRegisterMetaType<EtatPosition>("EtatPosition");
	qRegisterMetaType<EtatConsigne>("EtatConsigne");
	qRegisterMetaType<EtatArduino>("EtatArduino");
	qRegisterMetaType<EtatGuidage>("EtatGuidage");

	timerConnexionAuto.setInterval(PERIODE_ENTRE_CONNEXIONS);
	arduino = NULL;

	// Résultats de guidage
	consigne_c = consigne_l = 0;
	signalbruit = 1;
	afficherLesReperesDePosition = true;

	// Paramètres de guidage
	etatGuidage = GUIDAGE_ARRET_NORMAL;
	etatArduino = ARDUINO_CONNEXION_OFF;
	etatConsigne = CONSIGNE_NON_INITIALISEE;
	etatPosition = POSITION_NON_INITIALISEE;

	QObject::connect(&timerConnexionAuto, SIGNAL(timeout()), this,SLOT(connexionParTimer()));
	connexionParTimer(); // lancer les connexions au démarrage
	timerConnexionAuto.start();
	chargerParametres();

	// Ouverture du fichier de log (format : dd-MM-yyyy_hh-mm pos_x pos_y consigne_x consigne_y)
	// Il y a une entrée dans le log à chaque modification de la consigne
	fichier_log = new QFile("log_"+QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy")+".log");
	fichier_log->open(QIODevice::Text | QIODevice::ReadWrite);
	logPositions = new QTextStream(fichier_log);
}

Guidage::~Guidage() {
	enregistrerParametres();
	deconnecterArduino();
//	if(arduino) delete arduino; arduino=NULL;
//	if(img) delete img; img=NULL;
	cout << "Arduino déconnecté" << endl;
	// FIXME: Quand on a fait le quit(), les events sont tous traités ?
	logPositions->flush();
	fichier_log->close();
}

/**
 * Permet d'afficher ou masquer les repères (cercle avec une croix centrale)
 * @param afficher
 */
void Guidage::afficherReperesPositions(bool afficher) {
	afficherLesReperesDePosition = afficher;
}

/**
 * Méthode appelée régulièrement (à travers le timer timerConnexionAuto)
 * et vérifiant l'état de la connexion avec l'arduino
 */
void Guidage::connexionParTimer() {
	if (!arduinoConnecte()) {
		QStringList l = chercherFichiersArduino();
		if (l.length() > 0) {
			connecterArduino(l.first());
		} else {
			etatArduino = ARDUINO_FICHIER_INTROUV;
		}
	} else {
		etatArduino = ARDUINO_CONNEXION_ON;
	}
	if(etatArduino != ARDUINO_CONNEXION_ON) {
		etatGuidage = GUIDAGE_ARRET_PANNE;
		stopperGuidage(); // car il y a eu un arrêt de guidage
	}
	emit envoiEtatArduino(etatArduino);
}

/**
 * Demande de lancement du guidage
 */
void Guidage::lancerGuidage() {
	if (img.isNull() || position_c.isEmpty() || position_l.isEmpty()) {
		emit message("Impossible de guider, aucune position précédente");
		etatGuidage = GUIDAGE_BESOIN_POSITION;
		return;
	}
	else {
		etatGuidage = GUIDAGE_MARCHE;
	}
	if (consigne_c == 0 || consigne_l == 0) {
		consigneReset();
	}
	afficherImageSoleilEtReperes();
	tempsDepuisDernierGuidage.start();
}

/**
 * Demande d'arrêt du guidage
 */
void Guidage::stopperGuidage() {
	if(etatGuidage != GUIDAGE_MARCHE) { // Une autre méthode a modifié etatGuidage
		emit envoiEtatGuidage(etatGuidage);
	} else {
		emit envoiEtatGuidage(etatGuidage = GUIDAGE_ARRET_NORMAL);
	}
	decalage.clear(); // On vide l'historique des derniers décalages
}

/**
 * Recentrage de la consigne sur la position courante
 */
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

/**
 * Cette méthode est appelée lorsque le nombre de positions (et donc d'images capturées)
 * est suffisant ; ce test se fait dans la fonction qui reçoit les résultats de Capture.
 *
 * @note Durée d'envoi des impulsions : la méthode calcule combien de temps a duré la
 * dernière série de positions. Cette durée correspondra à la durée  maximale envoyée
 * à l'arduino en cas de décalage trop grand.
 *
 * @note Arrêt automatique en cas de divergence : la méthode historise les décalages qu'elle
 * a calculé et compare le décalage calculé il y a 10 itérations avec celui, en moyenne, entre
 * la 9ème et l'itération courante
 *
 * @note Choix de la direction : en fonction des parametres orientVertiInversee et
 * orientHorizInversee, la méthode inverse ou pas les commandes envoyées.
 */
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
	// Il y a divergence si la position t(-10) a produit un décalage
	// supérieur à la moyenne des positions t(-9..0)
	double somme = 0;
	if(decalage.length() >= 10) { // On
		for(i = decalage.length() - 9; i >= 0 && i < decalage.length(); i++) {
			somme += decalage.at(i);// Pour les 10 (ou moins) derniers décalages
		}
		somme = somme/9;
		if(somme > decalage.at(decalage.length() - 10) && arretSiEloignement) {
			emit message("Les commandes envoyées ne semblent pas avoir d'effet");
			emit message("Dernière commande envoyée : "+decalageTimestamp.at(decalageTimestamp.length()-1).toString("h:m:s"));
			emit envoiEtatGuidage(etatGuidage = GUIDAGE_ARRET_DIVERGE);
			stopperGuidage();
			afficherImageSoleilEtReperes();
			return;
		}
	}

	while(decalage.length() > 100) {
		decalage.removeFirst();
		decalageTimestamp.removeFirst();
	}
	while(position_c.length() > 100) {
		position_c.removeFirst();
		position_l.removeFirst();
	}

	emit message(
			"(x= " + QString::number(position_c.last()) + ", y="
			+ QString::number(position_l.last()) + ") "
			", (dx= " + QString::number(l_decal) + ", dy="
			+ QString::number(c_decal) + ")");

	int l_decal_duree = qAbs(l_decal * gainVertical);
	int c_decal_duree = qAbs(c_decal * gainHorizontal);

	// La duree entre deux corrections ne doit pas depasser la durée entre
	// deux séries de positions (tempsDepuisDernierGuidage ici)
	// NOTE: on évite les décalages > 5000ms
	if (l_decal_duree > tempsDepuisDernierGuidage.elapsed()
			|| l_decal_duree > DUREE_IMPULSION_MAX)
		l_decal_duree = min(tempsDepuisDernierGuidage.elapsed(),DUREE_IMPULSION_MAX);
	if (c_decal_duree > tempsDepuisDernierGuidage.elapsed()
			|| c_decal_duree > DUREE_IMPULSION_MAX)
		c_decal_duree = min(tempsDepuisDernierGuidage.elapsed(),DUREE_IMPULSION_MAX);

	// Envoi des commandes

	envoyerCmd((l_decal * (orientVertiInversee?-1:1) > 0
			? PIN_SUD	: PIN_NORD), l_decal_duree);
	envoyerCmd((c_decal * (orientHorizInversee?-1:1) > 0
			? PIN_OUEST	: PIN_EST), c_decal_duree);

	tempsDepuisDernierGuidage.restart();
}

/**
 * Connexion au fichier système "nom"
 * @param nom
 */
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
			etatArduino = ARDUINO_FICHIER_INTROUV;
		} else {
			etatArduino = ARDUINO_CONNEXION_OFF;
		}
		emit message(QString::fromStdString(arduino->getDerniereErreurMessage()));
	} else {
		etatArduino = ARDUINO_CONNEXION_ON;
		emit message("Arduino connecte ("+ QString::fromStdString(arduino->getPath()) + ")");
	}
	emit envoiEtatArduino(etatArduino);
}

/**
 * Déconnexion au fichier système
 */
void Guidage::deconnecterArduino() {
	if (arduino != NULL) {
		emit message("Le fichier " + QString::fromStdString(arduino->getPath())+ " a ete ferme");
		delete arduino;
		arduino = NULL;
	} else
		emit message("Aucun fichier n'etait ouvert");
}

/**
 * Vérifie la bonne connexion avec l'arduino
 * @note on pourrait utiliser arduino->verifierConnexion
 * qui envoie un caractère ENQ et attend un caractère ACK
 * pour vérifier si la connexion tient toujours ;
 * arduino->getErreur() permet seulement de savoir l'état du
 * fichier système
 * @return Si c'est connecté
 */
bool Guidage::arduinoConnecte() {
	return arduino && arduino->getErreur() == NO_ERR;
	// FIXME return arduino && arduino->verifierConnexion();
}

/**
 * Envoi d'une commande à l'arduino
 * @param pin
 * @param duree
 */
void Guidage::envoyerCmd(int pin, int duree) {
	string rep;
	if (arduinoConnecte()) {
		arduino->EnvoyerCmd(pin, duree);
		// l'arduino envoie des réponses aux commandes ; si on ne vide pas le buffer entrant,
		// l'arduino va se mettre à bloquer (TX et RX allumés en permanance) donc on flush
		arduino->Flush();
		emit message("Envoi impulsion pin " + QString::number(pin) + " et duree "+ QString::number(duree));
	}
}

/**
 * Cette méthode est un slot recevant le signal Capture::resultats(...)
 * envoyé par Capture dès qu'une image a été capturée et la position trouvée.
 * Chaque résultat est appelé "position".
 * @param img L'image envoyée par Capture
 * @param l
 * @param c
 * @param diametre
 * @param signalBruit
 */
void Guidage::traiterResultatsCapture(QImage img, double l, double c, int diametre, double signalbruit) {
	static int cptPositions = 0;
	this->diametre = diametre;
	position_l << l;
	position_c << c;
	this->img = QImage(img);
	this->signalbruit = signalbruit;


	// Si le signal/bruit est trop faible, il est possible qu'il s'agisse de nuages passagers.
	// On va donc passer au mode GUIDAGE_MARCHE_MAIS_BRUIT
	if (signalbruit < seuilSignalSurBruit) {
		etatPosition = POSITION_INCOHERANTE;
		if(etatGuidage == GUIDAGE_MARCHE) { // Signal/bruit vient de passer en dessous de la limite
			etatGuidage = GUIDAGE_MARCHE_MAIS_BRUIT;
			tempsDernierePositionCoherente.start();
		}
	}
	else {
		etatPosition = POSITION_COHERANTE;
	}

	if(etatGuidage == GUIDAGE_MARCHE_MAIS_BRUIT) { // Attente d'un signal/bruit plus élevé
		if(signalbruit > seuilSignalSurBruit) {
			// Le guidage peut reprendre
			etatGuidage = GUIDAGE_MARCHE;
		} else if(tempsDernierePositionCoherente.elapsed() > dureeApresMauvaisSignalBruit) {
			// Le temps maximal attendu après un mauvais bruit/signal est écoulé
			etatGuidage = GUIDAGE_ARRET_BRUIT;
		}
	}

	emit imageSoleil(img);
	emit envoiEtatGuidage(etatGuidage);
	emit signalBruit(signalbruit);
	emit envoiEtatPosition(etatPosition);
	emit envoiPositionCourante(position_c.last(),position_l.last());
	emit envoiEtatConsigne(etatConsigne);
	emit envoiPositionConsigne(consigne_c, consigne_l);
	afficherImageSoleilEtReperes();

	// Si l'historique des positions contient assez de poitions, on envoie le guidage
	if(etatGuidage == GUIDAGE_MARCHE && ++cptPositions == POSITIONS_PAR_GUIDAGE) {
		guider();
		cptPositions = 0;
	}
}

/**
 * Méthode "slot" permettant de changer la position de la consigne
 * @param deltaLigne
 * @param deltaColonne
 * @param decalageLent
 */
void Guidage::modifierConsigne(int deltaLigne, int deltaColonne, bool decalageLent) {
	if (consigne_l == 0 || consigne_c == 0 || img.isNull()) {
		emit message("Impossible de modifier la consigne : aucune position");
		return;
	}
	consigne_l = consigne_l+ deltaLigne* (decalageLent?INCREMENT_LENT:INCREMENT_RAPIDE);
	consigne_c = consigne_c+ deltaColonne* (decalageLent?INCREMENT_LENT:INCREMENT_RAPIDE);
	emit envoiEtatConsigne(etatConsigne);
	emit envoiPositionConsigne(consigne_c, consigne_l);
	afficherImageSoleilEtReperes();
	*logPositions << QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy_hh-mm")
			<< " "<< position_c.last() << " "<< position_l.last()
			<< " "<< consigne_c << " "<< consigne_l << "\n";
}

/**
 * Méthode pour chercher le nom du fichier système correspondant à l'arduino ;
 * on appelera ensuite connecterArduino() sur le premier de la liste
 * @return
 */
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

/**
 * Envoie un signal vers la fenêtre principale en demandant l'affichage
 * de l'image dans le WidgetImage
 */
void Guidage::afficherImageSoleilEtReperes() {
	emit imageSoleil(img);
	if(afficherLesReperesDePosition) {
		emit repereCourant(position_c.last()/img.width(),
			position_l.last()/img.height(),((float)diametre)/img.width(),etatPosition);
		emit repereConsigne(consigne_c/img.width(),
			consigne_l/img.height(),((float)diametre)/img.width(),etatConsigne);
	}
}
