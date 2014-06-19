#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"


#define DEV_DEFAULT "/dev/ttyACM0"



FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    paletteOk.setColor(QPalette::WindowText, Qt::green);
    palettePasOk.setColor(QPalette::WindowText, Qt::red);

    guidage = new Guidage;
    guidage->moveToThread(&threadGuidage);
    // Liens entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(guidage,SIGNAL(image(Image*)),ui->imageCamera,SLOT(afficherImage(Image*)));

    QObject::connect(this,SIGNAL(lancerGuidage(bool)),guidage,SLOT(lancerGuidage(bool)));

    QObject::connect(this,SIGNAL(connecterArduino(QString)),guidage,SLOT(connecterArduino(QString)));
    QObject::connect(this,SIGNAL(deconnecterArduino()),guidage,SLOT(deconnecterArduino()));
    QObject::connect(this,SIGNAL(connecterCamera()),guidage,SLOT(connecterCamera()));
    QObject::connect(this,SIGNAL(deconnecterCamera()),guidage,SLOT(deconnecterCamera()));
    QObject::connect(this,SIGNAL(demanderImage()),guidage,SLOT(demanderImage()));


    QObject::connect(guidage,SIGNAL(etatCamera(bool)),this,SLOT(statutCamera(bool)));
    QObject::connect(guidage,SIGNAL(etatArduino(bool)),this,SLOT(statutArduino(bool)));

    QObject::connect(ui->diametreSoleil,SIGNAL(valueChanged(int)),guidage,SLOT(initialiserDiametre(int)));


    QObject::connect(guidage,SIGNAL(consigne(double,double)),this,SLOT(afficherConsigne(double,double)));
    QObject::connect(ui->consigne_l,SIGNAL(valueChanged(double)),guidage,SLOT(consigneLigne(double)));
    QObject::connect(ui->consigne_c,SIGNAL(valueChanged(double)),guidage,SLOT(consigneColonne(double)));

    QObject::connect(guidage,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));


    // Liens entre fenetreprincipale et le thread guidage
    QObject::connect(&threadGuidage,SIGNAL(finished()),this,SLOT(guidageTermine()));

    ui->diametreSoleil->setValue(200);
    ui->nomFichierArduino->setText(DEV_DEFAULT);
    afficherMessage("Bienvenue");

    threadGuidage.start();


}

FenetrePrincipale::~FenetrePrincipale() {
    delete ui;
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
    emit demanderImage();
}

void FenetrePrincipale::on_connecterArduino_clicked() {
    emit connecterArduino(ui->nomFichierArduino->text());
}

void FenetrePrincipale::on_deconnecterArduino_clicked() {
    emit deconnecterArduino();
}

void FenetrePrincipale::on_lancerGuidage_clicked() {
    emit lancerGuidage(true);
}

void FenetrePrincipale::on_stopperGuidage_clicked() {
	emit lancerGuidage(false);
}

void FenetrePrincipale::guidageTermine() {
	afficherMessage("Le guidage est termine");
}

void FenetrePrincipale::statutCamera(bool etat) {
	if(etat) {
		ui->statutCamera->setPalette(paletteOk);
		ui->statutCamera->setText("Camera OK");
	}
	else {
		ui->statutCamera->setPalette(palettePasOk);
		ui->statutCamera->setText("Camera PAS OK");
	}
}

void FenetrePrincipale::statutArduino(bool etat) {
	if(etat) {
		ui->statutArduino->setPalette(paletteOk);
		ui->statutArduino->setText("Arduino OK");
	}
	else {
		ui->statutArduino->setPalette(palettePasOk);
		ui->statutArduino->setText("Arduino PAS OK");
	}
}

void FenetrePrincipale::afficherConsigne(double l, double c) {
	ui->consigne_l->setValue(l);
	ui->consigne_c->setValue(c);
}
