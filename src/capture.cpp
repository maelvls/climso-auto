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

#include "capture.h"

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
	img = NULL;
	ref_lapl = NULL;
	position_c = position_l = 0;
	diametre = 0;
	QObject::connect(&timerProchaineCapture,SIGNAL(timeout()),this,SLOT(captureEtPosition()), Qt::DirectConnection);
	timerProchaineCapture.setSingleShot(true);
	timerProchaineCapture.setInterval(DUREE_ENTRE_CAPTURES);
	qRegisterMetaType<EtatCamera>("EtatCamera");
	captureEtPosition(); // Lancement de la première capture
}

Capture::~Capture() {
	enregistrerParametres();
	camera.deconnecter();
	//  if(cam) delete cam; cam=NULL;
	//	if(img) delete img; img=NULL;
	//	if(ref_lapl) delete ref_lapl; ref_lapl = NULL;
	cout << "Caméra deconnectée" <<endl;
}

/**
 * Connecte la caméra
 */
void Capture::connecterCamera() {
	if(!camera.connecter() || !camera.estConnectee()) {
		emit message("Erreur lors de la connexion a la camera : " + QString::fromStdString(camera.derniereErreur()));
        etatCamera = CAMERA_CONNEXION_OFF;
	} else {
        emit message("Camera connectee");
        etatCamera = CAMERA_CONNEXION_ON;
    }
    emit envoiEtatCamera(etatCamera);
}

/**
 * Déconnecte la caméra
 */
void Capture::deconnecterCamera() {
	if(camera.deconnecter()) {
		emit message("Camera bien deconnectee");
		etatCamera = CAMERA_CONNEXION_OFF;
    } else {
        emit message("Aucune camera n'est connectee");
        etatCamera = CAMERA_CONNEXION_OFF;
    }
    emit envoiEtatCamera(etatCamera);
}

/**
 * Méthode appelée régulièrement et permettant de reconnecter
 * la caméra dans le cas d'une déconnexion
 */
void Capture::connexionAuto() {
	if(!camera.estConnectee()) {
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
	if((img = camera.capturer()) == NULL) {
		emit message("Impossible de lire capturer l'image : "+QString::fromStdString(camera.derniereErreur()));
		return false;
	}
    imgPourAffichage = versQImage(img);
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

#ifdef DEBUG
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

#ifdef DEBUG
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
