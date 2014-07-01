#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"


#define DEV_DEFAULT "/dev/ttyACM0"


FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);
    paletteTexteVert.setColor(QPalette::WindowText, Qt::green);
    paletteTexteRouge.setColor(QPalette::WindowText, Qt::red);
    paletteTexteJaune.setColor(QPalette::WindowText, Qt::yellow);

    guidage = new Guidage;
    guidage->moveToThread(&threadGuidage);
    capture = new Capture;
    capture->moveToThread(&threadCapture);

    // Liens entre fenetreprincipale et guidage
    QObject::connect(this,SIGNAL(deconnecterArduino()),guidage,SLOT(deconnecterArduino()));
    QObject::connect(guidage,SIGNAL(etatArduino(int)),this,SLOT(statutArduino(int)));
    QObject::connect(guidage,SIGNAL(etatGuidage(bool)),this,SLOT(statutGuidage(bool)));
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));
    QObject::connect(this,SIGNAL(stopperGuidage()),guidage,SLOT(stopperGuidage()));
    QObject::connect(this,SIGNAL(modificationConsigne(int,int)),guidage,SLOT(modifierConsigne(int,int)));
    QObject::connect(ui->initialiserConsigne,SIGNAL(clicked()),guidage,SLOT(initialiserConsigne()));
    QObject::connect(guidage,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));
    QObject::connect(&threadGuidage,SIGNAL(finished()),guidage,SLOT(deleteLater()));


    // Liens entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(etatCamera(int)),this,SLOT(statutCamera(int)));
    QObject::connect(this,SIGNAL(connecterCamera()),capture,SLOT(connecterCameraAuto()));
    QObject::connect(this,SIGNAL(deconnecterCamera()),capture,SLOT(deconnecterCameraAuto()));
    QObject::connect(ui->diametreSoleil,SIGNAL(valueChanged(int)),capture,SLOT(modifierDiametre(int)));
    QObject::connect(&threadCapture,SIGNAL(finished()),capture,SLOT(deleteLater()));


    // Liens entre capture et guidage
    QObject::connect(capture,SIGNAL(resultats(Image*,double,double,int,double)),guidage, SLOT(traiterResultatsCapture(Image*,double,double,int,double)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));

    // Liens avec imageCamera (le widget)
    QObject::connect(guidage,SIGNAL(imageSoleil(Image*)),ui->imageCamera,SLOT(afficherImageSoleil(Image*)));
    QObject::connect(guidage,SIGNAL(repereSoleil(float,float,float,QColor)),ui->imageCamera,SLOT(afficherRepere(float,float,float,QColor)));


    ui->diametreSoleil->setValue(200);

    threadGuidage.start();
    threadCapture.start();


    // Pour capturer les touches directionnelles du clavier pour controler la consigne
    ui->messages->installEventFilter(this);
    ui->consigneDroite->installEventFilter(this);
    ui->centralWidget->installEventFilter(this);
	ui->centralWidget->installEventFilter(this);
	ui->consigneBas->installEventFilter(this);
	ui->consigneDroite->installEventFilter(this);
	ui->consigneGauche->installEventFilter(this);
	ui->consigneHaut->installEventFilter(this);
	ui->diametreSoleil->installEventFilter(this);
	ui->imageCamera->installEventFilter(this);
	ui->initialiserConsigne->installEventFilter(this);
	ui->lancerGuidage->installEventFilter(this);
	ui->messages->installEventFilter(this);

}

FenetrePrincipale::~FenetrePrincipale() {
    delete ui;
}

void FenetrePrincipale::afficherMessage(QString msg) {
    ui->messages->append(msg);
}

void FenetrePrincipale::on_lancerGuidage_clicked() {
    emit lancerGuidage();
}

void FenetrePrincipale::on_stopperGuidage_clicked() {
	emit stopperGuidage();
}

void FenetrePrincipale::statutCamera(int etat) {
	if(etat & CONNEXION_CAMERA_OK) {
		ui->statutCamera->setPalette(paletteTexteVert);
		ui->statutCamera->setText("Camera OK");
	}
	else if(etat & CONNEXION_CAMERA_PAS_OK){
		ui->statutCamera->setPalette(paletteTexteRouge);
		ui->statutCamera->setText("Camera PAS OK");
	}
}

void FenetrePrincipale::statutArduino(int etat) {
	switch(etat) {
		case ARDUINO_CONNEXION_ON:
			ui->statutArduino->setPalette(paletteTexteVert);
			ui->statutArduino->setText("Arduino connecte");
			break;
		case ARDUINO_CONNEXION_OFF:
			ui->statutArduino->setPalette(paletteTexteJaune);
			ui->statutArduino->setText("Arduino deconnecte");
			break;
		case ARDUINO_FICHIER_INTROUV:
			ui->statutArduino->setPalette(paletteTexteRouge);
			ui->statutArduino->setText("Arduino introuvable");
	}
}

void FenetrePrincipale::on_consigneHaut_clicked() {
	emit modificationConsigne(-1,0);
}
void FenetrePrincipale::on_consigneBas_clicked() {
	emit modificationConsigne(1,0);
}
void FenetrePrincipale::on_consigneDroite_clicked() {
	emit modificationConsigne(0,1);
}
void FenetrePrincipale::on_consigneGauche_clicked() {
	emit modificationConsigne(0,-1);
}


void FenetrePrincipale::closeEvent(QCloseEvent* event) {
	threadCapture.quit(); // termine la boucle d'event de l'objet QThread
	threadGuidage.quit();
	threadGuidage.wait(); // Attend que le QThread soit terminé
	threadCapture.wait();
	cout << "Application terminée" << endl;
	// FIXME: quant on quitte comme ça, deleteLater() n'a pas le temps
	// de supprimer l'objet
}

void FenetrePrincipale::statutGuidage(bool statut) {
	if(statut) {
		ui->statutGuidage->setPalette(paletteTexteVert);
		ui->statutGuidage->setText("Marche");
	}
	else {
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arret");
	}
}

void FenetrePrincipale::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
		case Qt::Key_Up:
			emit modificationConsigne(-1,0);
			break;
		case Qt::Key_Down:
			emit modificationConsigne(1,0);
			break;
		case Qt::Key_Right:
			emit modificationConsigne(0,1);
			break;
		case Qt::Key_Left:
			emit modificationConsigne(0,-1);
			break;
	}
}


// Pour capturer les touches directionnelles du clavier pour controler la consigne
bool FenetrePrincipale::eventFilter(QObject *obj, QEvent *event)
 {
     if (		obj ==  ui->centralWidget
    		 || obj ==  ui->consigneBas
    		 || obj ==  ui->consigneDroite
    		 || obj ==  ui->consigneGauche
    		 || obj ==  ui->consigneHaut
    		 || obj ==  ui->diametreSoleil
    		 || obj ==  ui->imageCamera
    		 || obj ==  ui->initialiserConsigne
    		 || obj ==  ui->lancerGuidage
    		 || obj ==  ui->messages
    		 ) {
         if (event->type() == QEvent::KeyPress) {
             QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
             keyPressEvent(keyEvent);
             return true;
         } else {
             return false;
         }
     } else {
         //passer l'événement à la classe parent
         return QMainWindow::eventFilter(obj, event);
     }
}
