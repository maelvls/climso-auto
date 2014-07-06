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

#define POSITION_OK				Qt::green //QColor(qRgb(255,145,164))//QColor(qRgb(44,117,255))
#define POSITION_NOK			Qt::gray //QColor(qRgb(187,210,225))
#define CONSIGNE_OK				Qt::green //QColor(qRgb(86,130,3))
#define CONSIGNE_LOIN			Qt::yellow //QColor(qRgb(230,126,48))
#define CONSIGNE_DIVERGE		Qt::red

#define INCREMENT_LENT		0.1 // Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels
#define INCREMENT_RAPIDE	1	// Vitesse de déplacement de la consigne à chaque déplacement en nombre de pixels

// États possibles pour l'Arduino (permet de rafraichir le statut d'arduino dans la fenêtre)
#define ARDUINO_CONNEXION_ON		0
#define ARDUINO_CONNEXION_OFF		1
#define ARDUINO_FICHIER_INTROUV		2

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
	bool guidageEnMarche;
	int orientationNordSud; // 1 si l'orientation Nord-Sud est correcte, -1 si elle est inversée
	int orientationEstOuest; // 1 si l'orientation Est-Ouest est correcte, -1 si elle est inversée
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
	void etatArduino(int);
	void etatGuidage(bool);
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y,
			float diametre_pourcent_x, QColor couleur);
	void repereConsigne(float pourcent_x, float pourcent_y,
			float diametre_pourcent_x, QColor couleur);
	void signalBruit(double);
};

#endif /* GUIDAGE_H_ */
