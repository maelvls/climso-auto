/*
 * capture.cpp
 *
 *  Created on: 24 juin 2014
 *      Author: Maël Valais
 *
 *	Classe de traitement (contrairement à fenetreprincipale.cpp qui gère l'interface)
 *	gérant :
 *		- la connexion et déconnexion avec la caméra,
 *		- la capture des images avec la caméra,
 *		- le calcul de la position par corrélation.
 *
 *	Les résultats des captures sont envoyées à la classe Guidage (grâce au signal
 *	resultats(...)).
 *
 *	Cette classe gère aussi le diamètre du soleil (on lui envoie les nouveaux diamètres
 *	ou alors elle utilise le calcul du diamètre automatique de diametre_soleil.c)
 *
 *	L'instance de cette classe doit être exécuté dans un thread différent car dans le cas contraire,
 *	l'interface serait bloquée.
 *
 *
 */

#include <QtCore/QSettings>
// Les paramètres sont enregistrés (sous Linux) dans ~/.config/irap/climso-auto.conf

#include "capture.h"
#include "diametre_soleil.h"

/**
 * Paramètres chargés pour la capture (fichier ~/.config/irap/climso-auto.conf sous linux)
 * On peut modifier les paramètres par défaut en modifiant la valeur du
 * second paramètres de value(): value("le-nom-du-parametre",valeurParDéfaut)
 * Certains de ces paramètres sont aussi modifiables depuis le logiciel (menu > paramètres)
 */
void Capture::chargerParametres() {
	QSettings parametres("irap", "climso-auto");
	diametre = parametres.value("diametre-soleil-en-pixel", 275).toInt(); // diamètre du soleil en pixels
	normaliserImageAffichee = parametres.value("normaliser-image-affichee", true).toBool();
	modifierDiametre(diametre);
	emit diametreSoleil(diametre);
}

void Capture::enregistrerParametres() {
	QSettings parametres("irap", "climso-auto");
	parametres.setValue("diametre-soleil-en-pixel", diametre);
	parametres.setValue("normaliser-image-affichee",normaliserImageAffichee);
}

Capture::Capture() {
	cam = NULL;
	img = NULL;
	ref_lapl = NULL;
	position_c = position_l = 0;
	diametre = 0;
	QObject::connect(&timerProchaineCapture,SIGNAL(timeout()),this,SLOT(captureEtPosition()));
	timerProchaineCapture.setSingleShot(true);
	timerProchaineCapture.setInterval(DUREE_ENTRE_CAPTURES);
	qRegisterMetaType<EtatCamera>("EtatCamera");
	captureEtPosition(); // Lancement de la première capture
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
 * Connecte la caméra
 */
void Capture::connecterCamera() {
    PAR_ERROR err;
    if(cam != NULL)
        delete cam;
    cam = new CSBIGCam(DEV_USB); // Creation du device USB
    if ((err = cam->GetError()) != CE_NO_ERROR) {
        emit message("Erreur avec la camera lors de la creation de l'objet camera : "+QString::fromStdString(cam->GetErrorString()));
        etatCamera = CAMERA_CONNEXION_OFF;
    }
    else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
        emit message("Erreur avec la camera lors de l'etablissement du lien: "+QString::fromStdString(cam->GetErrorString()));
        etatCamera = CAMERA_CONNEXION_OFF;
    }
    else { // Pas d'erreurs, on met en binning 3x3
        cam->SetReadoutMode(RM_3X3);
        cam->SetExposureTime(DUREE_EXPOSITION * 0.001);
        cam->SetFastReadout(true);
        cam->SetABGState((ABG_STATE7)ABG_LOW7);

        emit message("Camera connectee");
        etatCamera = CAMERA_CONNEXION_ON;
    }
    emit envoiEtatCamera(etatCamera);
}

/**
 * Déconnecte la caméra
 */
void Capture::deconnecterCamera() {
    if(cam) {
        cam->CloseDevice();
        emit message("Camera deconnectee");
        delete cam; cam = NULL;
        etatCamera = CAMERA_CONNEXION_OFF;
    }
    else {
        emit message("Aucune camera n'est connectee");
        etatCamera = CAMERA_CONNEXION_OFF;
    }
    emit envoiEtatCamera(etatCamera);
}

/**
 * Vérifie la bonne connexion avec la caméra
 * @return
 */
bool Capture::cameraConnectee() {
    return cam && cam->EstablishLink() == CE_NO_ERROR;
}

/**
 * Méthode appelée régulièrement et permettant de reconnecter
 * la caméra dans le cas d'une déconnexion
 */
void Capture::connexionAuto() {
	if(!cameraConnectee()) {
		etatCamera = CAMERA_CONNEXION_OFF;
		emit stopperGuidage();
		connecterCamera();
	} else {
		etatCamera = CAMERA_CONNEXION_ON;
	}
	emit envoiEtatCamera(etatCamera);
}

/**
 * Conversion d'une Image en QImage pour l'affichage dans la fenêtre principale
 * cette image est "normalisée".
 * @param img
 * @return
 */
QImage Capture::versQImage(Image* img) {
	static unsigned char *img_uchar = NULL;
	if(img_uchar) delete img_uchar;
	if(normaliserImageAffichee) {
		img_uchar = img->versUcharEtNormaliser();
	} else {
		img_uchar = img->versUchar();
	}
	// Creation de l'index (34 va donner 34...) car Qt ne gère pas les nuances de gris
	QImage *temp = new QImage(img_uchar, img->getColonnes(), img->getLignes(),img->getColonnes(), QImage::Format_Indexed8);
	for(int i=0;i<256;++i) { // Pour construire une image en nuances de gris (n'existe pas sinon sous Qt)
		temp->setColor(i, qRgb(i,i,i));
	}
	return *temp;
}

/**
 * Etape de capture d'une image et de binning 2x2
 * @return
 */
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
    imgPourAffichage = versQImage(img);
	delete img_temp;
	return true;
}


/**
 * Etape de recherche de position
 */
void Capture::trouverPosition() {
	if(img == NULL) {
		return;
	}
	Image *obj_lapl = img->convoluerParDerivee();
	Image *correl = obj_lapl->correlation_rapide_centree(*ref_lapl, SEUIL_CORRELATION);
	correl->maxParInterpolation(&position_l, &position_c);
	signalbruit = correl->calculerSignalSurBruit(position_l,position_c);

#if DEBUG
	img->versTiff(emplacement+"t_obj.tif");
	correl->versTiff(emplacement+"t_correl.tif");
	obj_lapl->versTiff(emplacement+"t_obj_lapl.tif");
#endif

	delete correl;
	delete obj_lapl;
}

QTime t; // pour debug de durée de correl/capture
/**
 * Méthode appelée régulièrement par un timer et lançant
 * la capture puis la recherche de position, et envoyant
 * ensuite le résultat à l'instance de la classe Guidage
 */
void Capture::captureEtPosition() {
	connexionAuto();
	t.start();
	if(capturerImage() && ref_lapl) {
		cout << "Temps ecoulé après capture : " << t.elapsed() << "ms" <<endl;
		trouverPosition();
		cout << "Temps ecoulé après corrélation : " << t.elapsed() << "ms" <<endl;
		// ENVOI DES RESULTATS
		emit resultats(imgPourAffichage,position_l,position_c,diametre,signalbruit);
	}
	timerProchaineCapture.start();
}

/**
 * Slot de modification du diamètre du soleil
 * @param diametre
 */
void Capture::modifierDiametre(int diametre) {
	if(ref_lapl) delete ref_lapl;
	Image *ref = Image::tracerFormeSoleil(diametre);
	ref_lapl = ref->convoluerParDerivee();

#if DEBUG
	ref->versTiff(emplacement+"t_ref.tif");
	ref_lapl->versTiff(emplacement+"t_ref_lapl.tif");
#endif

	delete ref;
	this->diametre = diametre;
}

/**
 * Recherche le meilleur diamètre pour la recherche du centre du soleil,
 * il faut avoir déjà initialisé le diametre car ne recherche qu'autour
 * du diamètre déjà présent (5 pixels autour)
 * @return le diamètre
 */
int Capture::chercherDiametreProche() {
	double signalbruit_max = 0; // On recherche le meilleur signal/bruit possible
	double diametre_optimise;
	Image* ref;
	for(int diam = diametre-5; diam < diametre+5; diam++) {
		ref = Image::tracerFormeSoleil(diam);
		if(ref_lapl) delete ref_lapl;
		ref_lapl = ref->convoluerParDerivee();
		delete ref;
		trouverPosition();
		if(signalbruit > signalbruit_max) {
			signalbruit_max = signalbruit;
			diametre_optimise = diam;
		}
	}
	emit diametreSoleil(diametre_optimise);
	modifierDiametre(diametre_optimise);
	return diametre_optimise;
}
