/*
 * guidage.h
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 *
 */

#ifndef GUIDAGE_H_
#define GUIDAGE_H_

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QtCore/QMetaType>
#include "arduino.h"
#include "image.h"

#define IMPULSION_PIXEL_H	400 // en ms, durée d'impulsion envoyée
#define IMPULSION_PIXEL_V	800 //ms (2px pour 1sec)
#define PIN_NORD			12 // Numéro du pin sur lequel seront envoyées les commandes Nord
#define	PIN_SUD				11 // Numéro du pin sur lequel seront envoyées les commandes Sud
#define	PIN_EST				10 // Numéro du pin sur lequel seront envoyées les commandes Est
#define PIN_OUEST			9 // Numéro du pin sur lequel seront envoyées les commandes Ouest
#define ORIENTATION_NORD_SUD 		0 // 1 quand le nord correspond au nord, 0 sinon
#define ORIENTATION_EST_OUEST		0 // 1 quand l'est correspond à l'est, 0 sinon
#define SEUIL_BRUIT_SIGNAL			0.50 // Seuil Bruit/Signal au dessus lequel le guidage ne peut être effectué (nuages..)
#define ECHANTILLONS_PAR_GUIDAGE	3 	// Nombre d'échatillons (de captures) nécessaires avant de guider
#define PERIODE_ENTRE_CONNEXIONS	1000 // Période entre deux vérifications de connexion à l'arduino
#define SEUIL_DECALAGE_PIXELS		1.0 // Distance en pixels en dessous laquelle la position est estimée comme correcte

#define INCREMENT_LENT		0.1 // Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels
#define INCREMENT_RAPIDE	1	// Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels

typedef enum {
	POSITION_COHERANTE, // La position courante est fiable vis à vis du bruit/signal
	POSITION_INCOHERANTE, // La position n'est pas fiable
	POSITION_NON_INITIALISEE // La position n'a pas été initialiasée auparavant
} EtatPosition;
typedef enum {
	CONSIGNE_OK, // La consigne et la position sont superposés selon le seuil
	CONSIGNE_LOIN, // La consigne et la position sont éloignés (envoi de commandes)
	CONSIGNE_DIVERGE, // Éloignement non contrôlé entre consigne et position -> vérifier branchements
	CONSIGNE_NON_INITIALISEE // La consigne n'a pas été initialiasée auparavant
} EtatConsigne;
typedef enum {
	ARDUINO_CONNEXION_ON, // L'arduino est connecté
	ARDUINO_CONNEXION_OFF, // L'arduino est déconnecté
	ARDUINO_FICHIER_INTROUV // Impossible d'ouvrir/de lire le fichier Arduino
} EtatArduino;
typedef enum {
	GUIDAGE_MARCHE, // Le guidage est en marche
	GUIDAGE_BESOIN_POSITION, // Le guidage a besoin d'une position courante du soleil
	GUIDAGE_ARRET_BRUIT, // Le guidage s'est arrêté pour cause de non-fiabilité (bruit/signal élevé)
	GUIDAGE_ARRET_NORMAL, // Le guidage s'est arrêté à la demande de l'utilisateur
	GUIDAGE_ARRET_DIVERGE, // Le guidage s'est arrêté à cause de l'éloignement non contrôlé consigne/position
	GUIDAGE_ARRET_PANNE // Le guidage s'est arrêté car l'arduino ou la caméra fait défaut
} EtatGuidage;

Q_DECLARE_METATYPE(EtatGuidage);
Q_DECLARE_METATYPE(EtatArduino);
Q_DECLARE_METATYPE(EtatPosition);
Q_DECLARE_METATYPE(EtatConsigne);

class Guidage: public QObject {
	Q_OBJECT
private:
	QTimer timerConnexionAuto;
	Arduino* arduino;

	// Résultats des captures envoyées par Capture
	double consigne_l, consigne_c;
	Image* img;	// Image envoyée par la classe Capture par le signal resultat()
	QList<double> position_l, position_c; // Historique des positions
	double bruitsignal;
	int diametre; // Diametre du soleil en pixels pour l'affichage lorsqu'on utilisera repereSoleil(...)
	QList<double> decalage; // Vecteur décalage entre la consigne et la position
	QList<QTime> decalageTimestamp;

	// Paramètres de guidage
	EtatArduino etatArduino;
	EtatConsigne etatConsigne;
	EtatPosition etatPosition;
	EtatGuidage etatGuidage;
	bool orientHorizInversee;
	bool orientVertiInversee;

	QTime tempsDepuisDernierGuidage;


	void capturerImage();
	bool arduinoConnecte();
	void afficherImageSoleilEtReperes();
public:
	Guidage();
	~Guidage();
	static QStringList chercherFichiersArduino();
public slots:
	// guidage
	void lancerGuidage();
	void stopperGuidage();
	void modifierConsigne(int deltaLigne, int deltaColonne, bool decalageLent);
	void traiterResultatsCapture(Image* img, double l, double c, int diametre,double bruitsignal);
	// arduino
	void connecterArduino(QString nom);
	void deconnecterArduino();
	void envoyerCmd(int pin, int duree);
	void consigneReset();
	void enregistrerParametres();
	void chargerParametres();
private slots:
	void guider();
	void connexionAuto();
signals:
	void message(QString msg);
	void envoiEtatArduino(EtatArduino);
	void envoiEtatGuidage(EtatGuidage);
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y,float diametre_pourcent_x, EtatPosition);
	void repereConsigne(float pourcent_x, float pourcent_y,float diametre_pourcent_x, EtatConsigne);
	void signalBruit(double);
};

#endif /* GUIDAGE_H_ */
