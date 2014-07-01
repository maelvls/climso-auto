/*
 * guidage.h
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 *
 *  Cette classe contient les slots et signaux permettant de lancer l'execution de la boucle
 *  de guidage
 */

#ifndef GUIDAGE_H_
#define GUIDAGE_H_

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QFileInfo>
#include <QDir>
#include "arduino.h"
#include "image.h"


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
#define SEUIL_DECALAGE_PIXELS		1.0
// FIXME: si temps envoi arduino > timer, pbm (on limite ce temps à un peu au dessous de celui du timer)

#define POSITION_OK				Qt::green //QColor(qRgb(255,145,164))//QColor(qRgb(44,117,255))
#define POSITION_NOK			Qt::gray //QColor(qRgb(187,210,225))
#define CONSIGNE_OK				Qt::green //QColor(qRgb(86,130,3))
#define CONSIGNE_LOIN			Qt::yellow //QColor(qRgb(230,126,48))
#define CONSIGNE_DIVERGE		Qt::red

#define INCREMENT_LENT		0.1 // en nombre de pixels
#define INCREMENT_RAPIDE	1
#define VITESSE_LENTE		0
#define VITESSE_RAPIDE		1

// Du moins pire au pire
#define ARDUINO_CONNEXION_ON		0
#define ARDUINO_CONNEXION_OFF		1
#define ARDUINO_FICHIER_INTROUV		2

#define ARDUINO_CONNEXION_AUTO_ON		0
#define ARDUINO_CONNEXION_AUTO_OFF		1

class Guidage : public QObject {
	Q_OBJECT
private:
	QTimer timerCorrection;
	QTimer timerVerificationConnexions;
	Arduino* arduino;

	double consigne_l, consigne_c;
	double position_l, position_c;
	double decalage;
	int diametre; // Pour l'affichage
	Image* img;
	double bruitsignal;
	QList<double> historique_l;
	QList<double> historique_c;

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
// arduino
    void connecterArduino(QString nom);
    void deconnecterArduino();
	void envoyerCmd(int pin, int duree);
private slots:
	void guidageAuto();
	void initialiserConsigne();
	void connexionAuto();
	void traiterResultatsCapture(Image* img, double l, double c, int diametre, double bruitsignal);
	void modifierConsigne(int deltaLigne, int deltaColonne, int modeVitesse);
signals:
	void message(QString msg);
	void etatArduino(int);
	void etatGuidage(bool);
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y, float diametre_pourcent_x, QColor couleur);
	void repereConsigne(float pourcent_x, float pourcent_y, float diametre_pourcent_x, QColor couleur);
	void signalBruit(double);
};

#endif /* GUIDAGE_H_ */
