/*
 * capture.cpp
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 *
 *
 */

#include <QtCore/QSettings>
#include "capture.h"

#define DIAMETRE_DEFAUT			200  	// diamètre du soleil en pixels (par défaut)
#define DUREE_ENTRE_CAPTURES 	200 	// en ms, il faut aussi compter le temps passé à capturer ! (1100ms environ)
#define DUREE_EXPOSITION		100 	// en ms (FIXME: j'ai l'impression que cela ne change rien pour < 100ms)

#define DEBUG_IMAGES_CAMERA 	1		// Enregistrer les images capturées et traitées en .tif

#if DEBUG_IMAGES_CAMERA
string emplacement = ""; // Emplacement des images du debug caméra et corrélation (et laplacien)
#endif

QTime t;

Capture::Capture() {
	cam = NULL;
	img = NULL;
	ref_lapl = NULL;
	position_c = position_l = 0;
	diametre = 0;
	QObject::connect(&timerProchaineCapture,SIGNAL(timeout()),this,SLOT(captureEtPosition()));
	timerProchaineCapture.setSingleShot(true);
	timerProchaineCapture.setInterval(DUREE_ENTRE_CAPTURES);
}

Capture::~Capture() {
	enregistrerParametres();
	deconnecterCamera();
//	if(cam) delete cam; cam=NULL;
//	if(img) delete img; img=NULL;
//	if(ref_lapl) delete ref_lapl; ref_lapl = NULL;
	cout << "Caméra deconnectée" <<endl;
	// FIXME: Quand on a fait le quit(), les events sont tous traités ?
}

/**
 * Cette méthode permet de lancer les méthodes d'initialisation qui nécessitent l'envoi de signaux
 * et donc qui ne peuvent pas être fait durant la création de l'objet Capture dans le constructeur
 */
void Capture::initialiserObjetCapture() {
	lireParametres(); // Chargement des paramètres
	captureEtPosition(); // Lancement de la première capture
}

void Capture::enregistrerParametres() {
	QSettings parametres("irap", "climso-auto");
	parametres.setValue("diametre-soleil-en-pixel", diametre);
}

void Capture::lireParametres() {
	QSettings parametres("irap", "climso-auto");
	diametre = parametres.value("diametre-soleil-en-pixel", DIAMETRE_DEFAUT).toInt();
	modifierDiametre(diametre);
	emit diametreSoleil(diametre);
}

void Capture::connecterCamera() {
    PAR_ERROR err;
    if(cam != NULL)
        delete cam;
    cam = new CSBIGCam(DEV_USB); // Creation du device USB
    if ((err = cam->GetError()) != CE_NO_ERROR) {
        emit message("Erreur avec la camera lors de la creation de l'objet camera : "+QString::fromStdString(cam->GetErrorString()));
        emit etatCamera(CAMERA_CONNEXION_OFF);
    }
    else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
        emit message("Erreur avec la camera lors de l'etablissement du lien: "+QString::fromStdString(cam->GetErrorString()));
        emit etatCamera(CAMERA_CONNEXION_OFF);
    }
    else { // Pas d'erreurs, on met en binning 3x3
        cam->SetReadoutMode(RM_3X3);
        cam->SetExposureTime(DUREE_EXPOSITION * 0.001);
        cam->SetABGState(ABG_LOW7);

        emit message("Camera connectee");
        emit etatCamera(CAMERA_CONNEXION_ON);
    }
}

void Capture::deconnecterCamera() {
    if(cam) {
        cam->CloseDevice();
        emit message("Camera deconnectee");
        delete cam; cam = NULL;
        emit etatCamera(CAMERA_CONNEXION_OFF);
    }
    else {
        emit message("Aucune camera n'est connectee");
    }
}

bool Capture::cameraConnectee() {
    return cam && cam->EstablishLink() == CE_NO_ERROR;
}

void Capture::connexionAuto() {
	if(!cameraConnectee()) {
		emit etatCamera(CAMERA_CONNEXION_OFF);
		emit stopperGuidage();
		connecterCamera();
	} else {
		emit etatCamera(CAMERA_CONNEXION_ON);
	}
}

void Capture::lancerCapture() {
	timerProchaineCapture.start();
}
void Capture::stopperCapture() {
	timerProchaineCapture.stop();
}
bool Capture::capturerImage() {
    if (!cameraConnectee()) {
        emit message("La camera n'est pas connectee");
        return false;
    }
    CSBIGImg* img_sbig = new CSBIGImg();
    if(cam->GrabImage(img_sbig, SBDF_LIGHT_ONLY) != CE_NO_ERROR) {
        emit message("Impossible de lire capturer l'image : "+QString::fromStdString(cam->GetErrorString()));
        return false;
    }
    Image* img_temp = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig; // On supprime l'image CSBIGImg

    if(img) delete img; // On supprime la derniere image
    img = img_temp->reduire(2);
	delete img_temp;
	return true;
}

void Capture::trouverPosition() {
	if(diametre == 0)
		modifierDiametre(DIAMETRE_DEFAUT);
	if(img == NULL) {
		return;
	}
	Image *obj_lapl = img->convoluerParDerivee();
	Image *correl = obj_lapl->correlation_rapide_centree(*ref_lapl, 0.85);
	correl->maxParInterpolation(&position_l, &position_c);
	double bruitsignal = correl->calculerHauteurRelativeAutour(position_l,position_c);
	// ENVOI DES RESULTATS
    emit resultats(img,position_l,position_c,diametre,bruitsignal);

#if DEBUG_IMAGES_CAMERA
	img->versTiff(emplacement+"t0_obj.tif");
	correl->versTiff(emplacement+"t0_correl.tif");
	obj_lapl->versTiff(emplacement+"t0_obj_lapl.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete correl;
	delete obj_lapl;
}


void Capture::captureEtPosition() {
	connexionAuto();
	//t.start();
	if(capturerImage() && ref_lapl) {
    //cout << "Temps ecoulé après capture : " << t.elapsed() << "ms" <<endl;
	trouverPosition();
    //cout << "Temps ecoulé après corrélation : " << t.elapsed() << "ms" <<endl;
	}
	timerProchaineCapture.start();
	//emit diametreSoleil(diametre);
}

void Capture::modifierDiametre(int diametre) {
	cout << "modif de";
	if(ref_lapl) delete ref_lapl;
	Image *ref = Image::tracerFormeSoleil(diametre);
	ref_lapl = ref->convoluerParDerivee();

#if DEBUG_IMAGES_CAMERA
	ref->versTiff(emplacement+"t0_ref.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete ref;
	this->diametre = diametre;
}
