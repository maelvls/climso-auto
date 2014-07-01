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
	QString fichier_arduino;
	double consigne_l, consigne_c;
	double position_l, position_c;
	int diametre; // Pour l'affichage
	Image* img;

	void capturerImage();
    bool arduinoConnecte();
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
	void modifierConsigne(int deltaLigne, int deltaColonne);
signals:
	void repereSoleil(float pourcent_x, float pourcent_y, float diametre_pourcent_x);
	void message(QString msg);
	void etatArduino(int);
	void etatGuidage(bool);
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y, float diametre_pourcent_x, QColor couleur);
	void signalBruit(double);
};

#endif /* GUIDAGE_H_ */
