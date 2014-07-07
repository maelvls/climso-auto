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
#define PIN_NORD			12
#define	PIN_SUD				11
#define	PIN_EST				10
#define PIN_OUEST			9
#define ORIENTATION_NORD_SUD 		-1 // 1 quand le nord correspond au nord, -1 sinon
#define ORIENTATION_EST_OUEST		-1 // 1 quand l'est correspond à l'est, -1 sinon
#define SEUIL_BRUIT_SIGNAL			0.50 // Seuil Bruit/Signal au dessus lequel le guidage ne peut être effectué (nuages..)
#define ECHANTILLONS_PAR_GUIDAGE	3 	// Nombre d'échatillons (de captures) nécessaires avant de guider
#define PERIODE_ENTRE_CONNEXIONS	1000 // Période entre deux vérifications de connexion à l'arduino
#define SEUIL_DECALAGE_PIXELS		1.0 // Distance en pixels en dessous laquelle la position est estimée comme correcte



#define INCREMENT_LENT		0.1 // Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels
#define INCREMENT_RAPIDE	1	// Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels

enum EtatPosition {
	POSITION_COHERANTE,
	POSITION_INCOHERANTE,
	POSITION_NON_INITIALISEE
};
enum EtatConsigne {
	CONSIGNE_OK,
	CONSIGNE_LOIN,
	CONSIGNE_DIVERGE,
	CONSIGNE_NON_INITIALISEE
};
enum EtatArduino {
	ARDUINO_CONNEXION_ON,
	ARDUINO_CONNEXION_OFF,
	ARDUINO_FICHIER_INTROUV
};
enum EtatGuidage {
	GUIDAGE_MARCHE,
	GUIDAGE_BESOIN_POSITION,
	GUIDAGE_ARRET_BRUIT,
	GUIDAGE_ARRET_NORMAL,
	GUIDAGE_ARRET_DIVERGE,
	GUIDAGE_ARRET_PANNE
};

Q_DECLARE_METATYPE(enum EtatGuidage);
Q_DECLARE_METATYPE(enum EtatArduino);
Q_DECLARE_METATYPE(enum EtatPosition);
Q_DECLARE_METATYPE(enum EtatConsigne);

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
	int orientationNordSud; // 1 si l'orientation Nord-Sud est correcte, -1 si elle est inversée
	int orientationEstOuest; // 1 si l'orientation Est-Ouest est correcte, -1 si elle est inversée
	enum EtatArduino etatArduino;
	enum EtatConsigne etatConsigne;
	enum EtatPosition etatPosition;
	enum EtatGuidage etatGuidage;
	QTime tempsDepuisDernierGuidage;


	void capturerImage();
	bool arduinoConnecte();
	void afficherImageSoleilEtReperes();
	void enregistrerParametres();
	void lireParametres();
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
private slots:
	void guider();
	void connexionAuto();
signals:
	void message(QString msg);
	void envoiEtatArduino(enum EtatArduino);
	void envoiEtatGuidage(enum EtatGuidage);
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y,float diametre_pourcent_x, enum EtatPosition);
	void repereConsigne(float pourcent_x, float pourcent_y,float diametre_pourcent_x, enum EtatConsigne);
	void signalBruit(double);
};

#endif /* GUIDAGE_H_ */
