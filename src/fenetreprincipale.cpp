#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);
    cam = NULL;
    img_sbig = new CSBIGImg;
    img = NULL;
    img_affichee =NULL;
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}
void FenetrePrincipale::connecterCamera() {
    PAR_ERROR err;
    if(cam != NULL)
        delete cam;
    cam = new CSBIGCam(DEV_USB); // Creation du device USB
    if ((err = cam->GetError()) != CE_NO_ERROR) {
        afficherMessage("Erreur avec la camera lors de la creation de l'objet camera : "+cam->GetErrorString());
    }
    else if ((err=cam->EstablishLink()) != CE_NO_ERROR) { // Connexion à la camera
        afficherMessage("Erreur avec la camera lors de l'etablissement du lien: "+cam->GetErrorString());
    }
    else { // Pas d'erreurs, on met en binning 3x3
        cam->SetReadoutMode(RM_3X3);
        afficherMessage("Camera connectee");
    }
}

void FenetrePrincipale::deconnecterCamera() {
    if(cam) {
        cam->CloseDevice();
        afficherMessage("Camera deconnectee");
        delete cam; cam = NULL;
    }
    else {
        afficherMessage("Aucune camera n'est connectee");
    }

}
void FenetrePrincipale::capturerImage() {
    if (!cameraConnectee()) {
        afficherMessage("La camera n'est pas connectee");
        return;
    }
    if(cam->GrabImage(img_sbig, SBDF_DARK_ALSO) != CE_NO_ERROR) {
        afficherMessage("Impossible de lire capturer l'image : "+cam->GetErrorString());
        return;
    }
    if(img)
    	delete img;
    img = Image::depuisSBIGImg(*img_sbig);
    delete img_sbig;
    afficherImage(img);
    delete img;
}

void FenetrePrincipale::afficherImage(Image* img) {
    unsigned char *img_uchar = img->versUchar();

    // Creation de l'index (34 va donner 34...) car Qt ne gère pas les nuances de gris
    if(img_affichee) delete img_affichee;
    img_affichee = new QImage(img_uchar, img->getColonnes(), img->getLignes(),img->getColonnes(), QImage::Format_Indexed8);
    for(int i=0;i<256;++i) {
        img_affichee->setColor(i, qRgb(i,i,i));
    }

    QImage img_affichee_petite = img_affichee->scaled(ui->imageCamera->width(),ui->imageCamera->height(),Qt::KeepAspectRatio);
    ui->imageCamera->setPixmap(QPixmap::fromImage(img_affichee_petite,Qt::AutoColor));
    delete img_affichee_petite;
}

void FenetrePrincipale::afficherMessage(string err) {
    ui->messages->append(QString::fromStdString(err));
}

void FenetrePrincipale::on_connecterCamera_clicked() {
    connecterCamera();
}

void FenetrePrincipale::on_deconnecterCamera_clicked() {
    deconnecterCamera();
}

void FenetrePrincipale::on_capturerImage_clicked() {
    capturerImage();
    //essaiAffichageImage();
}

int FenetrePrincipale::cameraConnectee() {
    return cam && cam->CheckLink();
}

void guidageAuto() {
	// On utilise les variables globales x_consigne et y_consigne pour influer sur le thread de boucle
	QThread q = new QThread();
	q->
}
