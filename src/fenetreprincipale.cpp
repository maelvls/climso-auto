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
    capture = new Capture;
    capture->moveToThread(&threadCapture);

    // Liens entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(guidage,SIGNAL(etatArduino(bool)),this,SLOT(statutArduino(bool)));
    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));
    QObject::connect(this,SIGNAL(stopperGuidage()),guidage,SLOT(stopperGuidage()));
    QObject::connect(this,SIGNAL(consigneModification(int,int)),guidage,SLOT(consigneModifier(int,int)));
    QObject::connect(this,SIGNAL(deconnecterArduino()),guidage,SLOT(deconnecterArduino()));

    // Liens entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));
    QObject::connect(capture,SIGNAL(etatCamera(bool)),this,SLOT(statutCamera(bool)));
    QObject::connect(this,SIGNAL(deconnecterCamera()),capture,SLOT(deconnecterCamera()));

    // Liens entre capture et guidage
    QObject::connect(capture,SIGNAL(position(double,double,int,int,int)),guidage, SLOT(modifierPosition(double, double,int,int,int)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));
    QObject::connect(ui->diametreSoleil,SIGNAL(valueChanged(int)),capture,SLOT(modifierDiametre(int)));

    // Liens avec imageCamera (le widget)
    QObject::connect(capture,SIGNAL(imageSoleil(Image*)),ui->imageCamera,SLOT(afficherImageSoleil(Image*)));
    QObject::connect(capture,SIGNAL(repereSoleil(float,float,float)),ui->imageCamera,SLOT(afficherRepereCourant(float,float,float)));
    QObject::connect(guidage,SIGNAL(repereSoleil(float,float,float)),ui->imageCamera,SLOT(afficherRepereConsigne(float,float,float)));


    ui->diametreSoleil->setValue(200);
    ui->nomFichierArduino->setText(DEV_DEFAULT);

    threadGuidage.start();

    emit connecterArduino(DEV_DEFAULT);
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
    emit lancerGuidage();
}

void FenetrePrincipale::on_stopperGuidage_clicked() {
	emit stopperGuidage();
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

void FenetrePrincipale::on_consigneHaut_clicked() {
	emit consigneModification(-1,0);
}
void FenetrePrincipale::on_consigneBas_clicked() {
	emit consigneModification(1,0);
}
void FenetrePrincipale::on_consigneDroite_clicked() {
	emit consigneModification(0,1);
}
void FenetrePrincipale::on_consigneGauche_clicked() {
	emit consigneModification(0,-1);
}

void FenetrePrincipale::signalHandler(int signal)
{
    switch(signal){
        case SIGINT:
        case SIGKILL:
        case SIGQUIT:
        case SIGSTOP:
        case SIGTERM:
        case SIGSEGV:
        	cout << "Guidage arrete" << endl;
        	emit deconnecterCamera();
        	emit deconnecterArduino();
        	break;
        default: printf("APPLICATION EXITING => "); break;
    }
}

void FenetrePrincipale::closeEvent(QCloseEvent* event) {
	event = NULL;
	emit deconnecterArduino();
	emit deconnecterCamera();
	cout << "Guidage termine" << endl;
}

void FenetrePrincipale::statutGuidage(bool statut) {
	if(statut) {
		ui->statutGuidage->setPalette(paletteOk);
		ui->statutGuidage->setText("Marche");
	}
	else {
		ui->statutGuidage->setPalette(palettePasOk);
		ui->statutGuidage->setText("Arret");
	}
}
