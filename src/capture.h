/*
 * capture.h
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_

#include <QtCore/qobject.h>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QMetaType>

#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"

enum EtatCamera {
	CAMERA_CONNEXION_ON,
	CAMERA_CONNEXION_OFF
};
Q_DECLARE_METATYPE(enum EtatCamera); // Pour utiliser ce type comme parametre signal/slot

class Capture: public QObject {
	Q_OBJECT
private:
	QTimer timerConnexion;
	QTimer timerProchaineCapture;
	CSBIGCam* cam;
	//CSBIGImg* img_sbig;
	Image* img;
	Image* ref_lapl;// laplacien de la ref de l'image du soleil
	int diametre; 	// diametre du soleil en pixels
    double position_l;
    double position_c;

    bool cameraConnectee();
    void capturerEtTrouverPosition();
	void enregistrerParametres();
	void lireParametres();
public:
	Capture();
	virtual ~Capture();
public slots:
	void modifierDiametre(int diametre);
	void lancerCapture();
	void stopperCapture();
	void connecterCamera();
	void deconnecterCamera();
	bool capturerImage();
	void trouverPosition();
	void initialiserObjetCapture();
private slots:
	void connexionAuto();
	void captureEtPosition();
signals:
	void message(QString);
	void stopperGuidage();
	void envoiEtatCamera(enum EtatCamera);
	void resultats(Image* img, double l, double c, int diametre, double bruitsignal);
	void diametreSoleil(int);
};
#endif /* CAPTURE_H_ */
