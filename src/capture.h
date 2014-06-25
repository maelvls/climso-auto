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


/*
 * Si bruit/signal > 0.4 on n'affiche plus la correlation
 */

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
	void deconnecterCamera();
	void capturerImage();
	void trouverPosition();
private slots:
	void connexionAuto();
	void captureEtPosition();
signals:
	void imageSoleil(Image*);
	void repereSoleil(float pourcent_x, float pourcent_y, float diametre_pourcent_x);
	void messageCamera(PAR_ERROR, QString);
	void message(QString);
	void signalBruit(double);
	void stopperGuidage();
	void etatCamera(bool);
	void position(double l, double c, int l_max, int c_max, int diametre);
};

#endif /* CAPTURE_H_ */
