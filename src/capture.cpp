/*
 * capture.cpp
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 *
 *
 */

#include "capture.h"

#define DIAMETRE_DEFAUT		200
#define SEUIL_BRUIT_SIGNAL	0.40
#define PERIODE_CAPTURE		1500 // en ms
#define PERIODE_CONNEXION	1000 // en ms

static string emplacement = "";
QTime t;

Capture::Capture() {
	timerCapture.setInterval(PERIODE_CAPTURE);
	timerConnexion.setInterval(PERIODE_CONNEXION);
	cam = NULL;
	img = NULL;
	ref_lapl = NULL;
	position_c = position_l = 0;
	diametre = 0;
	QObject::connect(&timerConnexion,SIGNAL(timeout()),this,SLOT(connexionAuto()));
	QObject::connect(&timerCapture,SIGNAL(timeout()),this,SLOT(captureEtPosition()));
	timerConnexion.start();
	timerCapture.start();
	connecterCamera();
}

Capture::~Capture() {
}

void Capture::connecterCamera() {
    PAR_ERROR err;
    if(cam != NULL)
        delete cam;
    cam = new CSBIGCam(DEV_USB); // Creation du device USB
    if ((err = cam->GetError()) != CE_NO_ERROR) {
        emit message("Erreur avec la camera lors de la creation de l'objet camera : "+QString::fromStdString(cam->GetErrorString()));
        emit etatCamera(false);
    }
    else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
        emit message("Erreur avec la camera lors de l'etablissement du lien: "+QString::fromStdString(cam->GetErrorString()));
        emit etatCamera(false);
    }
    else { // Pas d'erreurs, on met en binning 3x3
        cam->SetReadoutMode(RM_3X3);
        cam->SetExposureTime(0.01);
        emit message("Camera connectee");
        emit etatCamera(true);
    }
}

void Capture::deconnecterCamera() {
    if(cam) {
        cam->CloseDevice();
        emit message("Camera deconnectee");
        delete cam; cam = NULL;
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
		emit etatCamera(false);
		connecterCamera();
	} else {
		emit etatCamera(true);
	}
}

void Capture::lancerCapture() {
	timerCapture.start();
}
void Capture::stopperCapture() {
	timerCapture.stop();
}
void Capture::capturerImage() {
    if (!cameraConnectee()) {
        emit message("La camera n'est pas connectee");
        return;
    }
    CSBIGImg* img_sbig = new CSBIGImg();
    if(cam->GrabImage(img_sbig, SBDF_LIGHT_ONLY) != CE_NO_ERROR) {
        emit message("Impossible de lire capturer l'image : "+QString::fromStdString(cam->GetErrorString()));
        return;
    }
    Image* img_temp = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig; // On supprime l'image CSBIGImg

    if(img) delete img; // On supprime la derniere image
    img = img_temp->reduire(2);
	delete img_temp;
	// ENVOI DES RESULTATS
	emit imageSoleil(img);
    //QCoreApplication::processEvents();
}

void Capture::trouverPosition() {
	if(diametre == 0)
		modifierDiametre(DIAMETRE_DEFAUT);
	if(img == NULL) {
		return;
	}
	Image *obj_lapl = img->convoluerParDerivee();
	Image *correl = obj_lapl->correlation_rapide_centree(*ref_lapl, 0.80);
	correl->maxParInterpolation(&position_l, &position_c);
	double ratio = correl->calculerHauteurRelativeAutour(position_l,position_c);

	// ENVOI DES RESULTATS
	emit signalBruit(ratio);
	if(ratio < SEUIL_BRUIT_SIGNAL) {
		emit repereSoleil(position_c/correl->getColonnes(),position_l/correl->getLignes(),((float)diametre)/correl->getColonnes());
		emit position(position_l,position_c,correl->getLignes(),correl->getColonnes(),diametre);
	} else {
		emit stopperGuidage();
	}

#if DEBUG
	img->versTiff(emplacement+"t0_obj.tif");
	correl->versTiff(emplacement+"t0_correl.tif");
	obj_lapl->versTiff(emplacement+"t0_obj_lapl.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete correl;
	delete obj_lapl;
    //QCoreApplication::processEvents();
}

void Capture::captureEtPosition() {
	t.start();
	capturerImage();
    cout << "Temps ecoulé : " << t.elapsed() << "ms" <<endl;

	trouverPosition();
    cout << "Temps ecoulé 2 : " << t.elapsed() << "ms" <<endl;

}

void Capture::modifierDiametre(int diametre) {
	if(ref_lapl) delete ref_lapl;
	Image *ref = Image::tracerFormeSoleil(diametre);
	ref_lapl = ref->convoluerParDerivee();

#if DEBUG
	ref->versTiff(emplacement+"t0_ref.tif");
	ref_lapl->versTiff(emplacement+"t0_ref_lapl.tif");
#endif

	delete ref;
	this->diametre = diametre;
	if(position_c > 0 && position_l > 0) {
		emit repereSoleil(position_c/img->getColonnes(),position_l/img->getLignes(),((float)diametre)/img->getColonnes());
	}
}

