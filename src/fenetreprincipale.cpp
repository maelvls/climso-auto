#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"


#define DEV_DEFAULT "/dev/ttyACM0"



FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);
    img_affichee = NULL;

    ui->nomFichierArduino->setText(DEV_DEFAULT);
    afficherMessage("Bienvenue");

    guidage = new Guidage;
    guidage->moveToThread(&threadGuidage);
    // Liens entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(guidage,SIGNAL(image(Image*)),this,SLOT(afficherImage(Image*)));

    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));

    QObject::connect(this,SIGNAL(connecterArduino(QString)),guidage,SLOT(connecterArduino(QString)));
    QObject::connect(this,SIGNAL(deconnecterArduino()),guidage,SLOT(deconnecterArduino()));
    QObject::connect(this,SIGNAL(connecterCamera()),guidage,SLOT(connecterCamera()));
    QObject::connect(this,SIGNAL(deconnecterCamera()),guidage,SLOT(deconnecterCamera()));



    // Liens entre fenetreprincipale et le thread guidage
    QObject::connect(&threadGuidage,SIGNAL(finished()),this,SLOT(guidageTermine()));
    threadGuidage.start();
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
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
}

void FenetrePrincipale::afficherMessage(QString msg) {
    ui->messages->append(msg);
}

void FenetrePrincipale::on_connecterCamera_clicked() {
    emit connecterCamera();
}

void FenetrePrincipale::on_deconnecterCamera_clicked() {
    emit deconnecterCamera();
}

void FenetrePrincipale::on_capturerImage_clicked() {
    emit capturerImage();
}

void FenetrePrincipale::on_connecterArduino_clicked() {
    emit connecterArduino(ui->nomFichierArduino->text());
}

void FenetrePrincipale::on_deconnecterArduino_clicked() {
    emit deconnecterArduino();
}

void FenetrePrincipale::on_lancerGuidage_clicked() {
    emit lancerGuidage();
}

void FenetrePrincipale::on_stopperGuidage_clicked() {

}

void FenetrePrincipale::guidageTermine() {
	afficherMessage("Le guidage est termine");
}

