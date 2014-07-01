/*
 * capture.h
 *
 *  Created on: 24 juin 2014
 *      Author: admin
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_

#include <QtCore/qobject.h>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>

#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"

#define CAMERA_CONNEXION_ON		1
#define CAMERA_CONNEXION_OFF	2

class Capture: public QObject {
	Q_OBJECT
private:
	QTimer timerConnexion;
	QTimer timerCapture;
	CSBIGCam* cam;
	//CSBIGImg* img_sbig;
	Image* img;
	Image* ref_lapl;// laplacien de la ref de l'image du soleil
	int diametre; 	// diametre du soleil en pixels
    double position_l;
    double position_c;

    bool cameraConnectee();
    void capturerEtTrouverPosition();

public:
	Capture();
	virtual ~Capture();
public slots:
	void modifierDiametre(int diametre);
	void lancerCapture();
	void stopperCapture();
	void connecterCamera();
	void connecterCameraAuto();
	void deconnecterCamera();
	void deconnecterCameraAuto();
	bool capturerImage();
	void trouverPosition();
private slots:
	void connexionAuto();
	void captureEtPosition();
signals:
	void messageCamera(PAR_ERROR, QString);
	void message(QString);
	void stopperGuidage();
	void etatCamera(int);
	void resultats(Image* img, double l, double c, int diametre, double bruitsignal);
};

#endif /* CAPTURE_H_ */
