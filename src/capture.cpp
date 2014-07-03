/*
 * capture.cpp
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 *
 *
 */

#include "capture.h"

#define DIAMETRE_DEFAUT			200  // diamètre du soleil par défaut
#define PERIODE_CONNEXION		1000 // en ms, inutilisé (connexion dans captureEtPosition())
#define PERIODE_ENTRE_CAPTURES 	1 // en ms

static string emplacement = "";
QTime t;

Capture::Capture() {
	timerConnexion.setInterval(PERIODE_CONNEXION);
	cam = NULL;
	img = NULL;
	ref_lapl = NULL;
	position_c = position_l = 0;
	diametre = 0;
	QObject::connect(&timerConnexion,SIGNAL(timeout()),this,SLOT(connexionAuto()));
	QObject::connect(&timerCapture,SIGNAL(timeout()),this,SLOT(captureEtPosition()));
	timerCapture.start(PERIODE_ENTRE_CAPTURES);
	//connecterCameraAuto(); XXX La connexion se fait lors de l'appel de captureEtPosition()
}

Capture::~Capture() {
	deconnecterCamera();
//	if(cam) delete cam; cam=NULL;
//	if(img) delete img; img=NULL;
//	if(ref_lapl) delete ref_lapl; ref_lapl = NULL;
	cout << "Caméra deconnectée" <<endl;
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
        cam->SetExposureTime(0.001);
        cam->SetABGState(ABG_LOW7);

        emit message("Camera connectee");
        emit etatCamera(CAMERA_CONNEXION_ON);
    }
}

void Capture::connecterCameraAuto() {
	connecterCamera();
	timerConnexion.start();
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

void Capture::deconnecterCameraAuto() {
	timerConnexion.stop();
	QCoreApplication::flush();
	deconnecterCamera();
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
	timerCapture.start();
}
void Capture::stopperCapture() {
	timerCapture.stop();
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
	Image *correl = obj_lapl->correlation_rapide_centree(*ref_lapl, 0.80);
	correl->maxParInterpolation(&position_l, &position_c);
	double bruitsignal = correl->calculerHauteurRelativeAutour(position_l,position_c);
	// ENVOI DES RESULTATS
    emit resultats(img,position_l,position_c,diametre,bruitsignal);

#if DEBUG
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
	//QCoreApplication::processEvents();



    // --------- XXX ----------
    struct timeval start, end;
    long mtime, seconds, useconds;
    gettimeofday(&start, NULL);
    // --------- --- ----------


	t.start();

	if(capturerImage()) {
    cout << "Temps ecoulé après capture : " << t.elapsed() << "ms" <<endl;
	trouverPosition();
    cout << "Temps ecoulé après corrélation : " << t.elapsed() << "ms" <<endl;

    // --------- XXX ----------
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Elapsed time: %ld milliseconds\n", mtime);
    // --------- --- ----------

	}
	timerCapture.start(PERIODE_ENTRE_CAPTURES);
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
}
