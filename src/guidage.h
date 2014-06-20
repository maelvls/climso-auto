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
#include "image.h"
#include "arduino.h"
#include "csbigcam.h"
#include "csbigimg.h"

class Guidage : public QObject {
	Q_OBJECT
private:
	QTimer timerCorrection;
	QTimer timerEchantillon;
	QThread threadDuGuidage;
	CSBIGCam* cam;
	CSBIGImg* img_sbig;
	Image* img;
	Arduino* arduino;
	double consigne_l;
	double consigne_c;
	Image* ref_lapl;
	int diametre; // diametre
	QTimer timerVerificationConnexions;
	void capturerImage();

public:
	Guidage();
public slots:
// guidage
	void lancerGuidage(bool);
// camera
    void connecterCamera();
    void deconnecterCamera();
	void demanderImage();
// arduino
    void connecterArduino(QString nom);
    void deconnecterArduino();
	void envoyerCmd(int pin, int duree);
	void initialiserDiametre(int diametre);

private slots:
	void guidageSuivant();
	void guidageInitial();
    bool cameraConnectee();
    bool arduinoConnecte();
	void verifierLesConnexions();
	//void changerDureeEchantillonage(int dureems);
	//void changerDureeCorrection(int dureems);
	void consigneModifier(int deltaLigne, int deltaColonne);
signals:
	void image(Image *img);
	void cercle(float pourcent_x, float pourcent_y, float diametre_pourcent_x);
	void message(QString msg);
	void etatArduino(bool);
	void etatCamera(bool);
	void consigne(double l, double c);
	void signalBruit(double ratio);
};

#endif /* GUIDAGE_H_ */
