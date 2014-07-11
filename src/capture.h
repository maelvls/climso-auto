/*
 * capture.h
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_


#define DIAM_MIN_RECHERCHE		150 // Diamètre minimal lorsqu'il y a recherche du diamètre du soleil
#define DIAM_MAX_RECHERCHE		300 // Diamètre maximal

#include <QtCore/qobject.h>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QMetaType>
#include <QtGui/QImage>

#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"



typedef enum {
	CAMERA_CONNEXION_ON,
	CAMERA_CONNEXION_OFF
} EtatCamera;

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
    double bruitsignal;

    bool cameraConnectee();
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
	void enregistrerParametres();
	void chargerParametres();
	int chercherDiametre();
private slots:
	void connexionAuto();
	void captureEtPosition();
signals:
	void message(QString);
	void stopperGuidage();
	void envoiEtatCamera(EtatCamera);
	void resultats(QImage img, double l, double c, int diametre, double bruitsignal);
	void diametreSoleil(int);
};

Q_DECLARE_METATYPE(EtatCamera); // Pour utiliser ce type comme parametre signal/slot

#endif /* CAPTURE_H_ */
