/*
 * capture.h
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_
#define DEBUG

/* 	Pour les paramètres: certains paramètres sont à configurer dans le fichier
 * 		~/.config/irap/climso-auto.conf (ou directement depuis menu > paramètres)
 * 	Paramètres par défaut: concernant ces réglages peuvent se trouver dans
 * 		la fonction chargerParametres()
 */

// Paramètres "en dur" ne pouvant être modifiés que par la recompilation :
#define SEUIL_CORRELATION		0.90 	// entre 0 et 1 (% du max de ref) au dessus duquel les valeurs de ref. sont correlées
#define DUREE_ENTRE_CAPTURES 	1000 	// en ms, il faut aussi compter le temps passé à capturer ! (1100ms environ)

#include "camera.h"
#include "camera_fake.h"
#include "camera_sbig.h"

#include <QtCore/qobject.h>
#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QMetaType>
#include <QtGui/QImage>
#include <QtCore/QSettings>
// Les paramètres sont enregistrés (sous Linux) dans ~/.config/irap/climso-auto.conf

#include "diametre_soleil.h"
#include "image.h"

// ACTIVER LE DEBUG: ./configure CPPFLAGS="-DDEBUG=1" (si autotools) ou gcc -DDEBUG=1 sinon
#ifdef DEBUG // Il faut activer la macro DEBUG pour enregistrer les images capturées et traitées en .tif
static string emplacement = ""; // Emplacement des images du debug caméra et corrélation (et laplacien)
#endif

typedef enum {
	CAMERA_CONNEXION_ON,
	CAMERA_CONNEXION_OFF
} EtatCamera;

class Capture: public QObject {
	Q_OBJECT
private:
	QTimer timerConnexion;
	QTimer timerProchaineCapture;

// The FAKE_CAMERA can be turned on using ./configure --enable-fake-camera
#ifdef FAKE_CAMERA
	CameraFake camera;
#else
	CameraSBIG camera;
#endif
	Image* img;
	Image* ref_lapl;// laplacien de la ref de l'image du soleil
	QImage imgPourAffichage; // Image non binnée pour envoi à Guidage
	int diametre; 	// diametre du soleil en pixels
    double position_l;
    double position_c;
    double signalbruit;
    bool normaliserImageAffichee;
    QImage versQImage(Image*);
    EtatCamera etatCamera;
public:
	Capture();
	virtual ~Capture();
public slots:
	void modifierDiametre(int diametre);
	void connecterCamera();
	void deconnecterCamera();
	bool capturerImage();
	void trouverPosition();
	void enregistrerParametres();
	void chargerParametres();
	int chercherDiametreProche();
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
