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

#include <QtCore/qobject.h>
#include <Qt/qtimer.h>
#include <Qt/qthread.h>
#include "image.h"
#include "arduino.h"
#include "csbigcam.h"
#include "csbigimg.h"

class Guidage : public QObject {
	Q_OBJECT
private:
	QTimer timer;
	QThread threadDuGuidage;
	double consigne_x;
	double consigne_y;
	CSBIGCam* cam;
	CSBIGImg* img_sbig;
	Image* img;
	Arduino* arduino;
	int duree;

	QTimer timerVerificationConnexions;

	void verifierLesConnexions();

public:
	Guidage();
public slots:
// guidage
	void lancerGuidage();
// camera
    void connecterCamera();
    void deconnecterCamera();
	void capturerImage();
// arduino
    void connecterArduino(QString nom);
    void deconnecterArduino();
	void envoyerCmd(int pin, int duree);

private slots:
	void guidageSuivant();
    bool cameraConnectee();
    bool arduinoConnecte();

signals:
	void image(Image *img);
	void message(QString msg);
	void etatArduino(bool);
	void etatCamera(bool);
};

#endif /* GUIDAGE_H_ */
