#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"
#include <QtCore/QTextCodec>


FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);
    paletteTexteVert.setColor(QPalette::WindowText, Qt::green);
    paletteTexteRouge.setColor(QPalette::WindowText, Qt::red);
    paletteTexteJaune.setColor(QPalette::WindowText, Qt::yellow);
    // Faire en sorte que les chaines statiques "C-strings" soient considérées comme UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    guidage = new Guidage; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    guidage->moveToThread(&threadGuidage);
    capture = new Capture; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    capture->moveToThread(&threadCapture);

    // Liens entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(etatArduino(int)),this,SLOT(statutArduino(int)));
    QObject::connect(guidage,SIGNAL(etatGuidage(bool)),this,SLOT(statutGuidage(bool)));
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));
    QObject::connect(this,SIGNAL(stopperGuidage()),guidage,SLOT(stopperGuidage()));
    QObject::connect(this,SIGNAL(modificationConsigne(int,int,int)),guidage,SLOT(modifierConsigne(int,int,int)));
    QObject::connect(ui->initialiserConsigne,SIGNAL(clicked()),guidage,SLOT(initialiserConsigne()));
    QObject::connect(guidage,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));
    QObject::connect(&threadGuidage,SIGNAL(finished()),guidage,SLOT(deleteLater()));


    // Liens entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(etatCamera(int)),this,SLOT(statutCamera(int)));
    QObject::connect(capture,SIGNAL(diametreSoleil(int)),ui->valeurDiametreSoleil,SLOT(setValue(int)));
    QObject::connect(this,SIGNAL(diametreSoleil(int)),capture,SLOT(modifierDiametre(int)));
    QObject::connect(this,SIGNAL(initialiserCapture()),capture,SLOT(initialiserObjetCapture()));


    QObject::connect(&threadCapture,SIGNAL(finished()),capture,SLOT(deleteLater()));


    // Liens entre capture et guidage
    QObject::connect(capture,SIGNAL(resultats(Image*,double,double,int,double)),guidage, SLOT(traiterResultatsCapture(Image*,double,double,int,double)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));


    // Liens entre les éléments de l'interface
    QObject::connect(ui->vitesseDecalageLent,SIGNAL(toggled(bool)),this,SLOT(modifierVitesseDeplacement()));
    QObject::connect(guidage,SIGNAL(imageSoleil(Image*)),ui->imageCamera,SLOT(afficherImageSoleil(Image*)));
    QObject::connect(guidage,SIGNAL(repereConsigne(float,float,float,QColor)),ui->imageCamera,SLOT(afficherRepereConsigne(float,float,float,QColor)));
    QObject::connect(guidage,SIGNAL(repereSoleil(float,float,float,QColor)),ui->imageCamera,SLOT(afficherRepereSoleil(float,float,float,QColor)));

    threadGuidage.start();
    threadCapture.start();
    emit initialiserCapture();

    ui->vitesseDecalageRapide->setChecked(true);

    // Pour capturer les touches directionnelles du clavier pour controler la consigne
    ui->messages->installEventFilter(this);
    ui->consigneDroite->installEventFilter(this);
    ui->centralWidget->installEventFilter(this);
	ui->centralWidget->installEventFilter(this);
	ui->consigneBas->installEventFilter(this);
	ui->consigneDroite->installEventFilter(this);
	ui->consigneGauche->installEventFilter(this);
	ui->consigneHaut->installEventFilter(this);
	ui->valeurDiametreSoleil->installEventFilter(this);
	ui->diametreSoleilValider->installEventFilter(this);
	ui->imageCamera->installEventFilter(this);
	ui->initialiserConsigne->installEventFilter(this);
	ui->lancerGuidage->installEventFilter(this);
	ui->messages->installEventFilter(this);
	ui->vitesseDecalageLent->installEventFilter(this);
	ui->vitesseDecalageRapide->installEventFilter(this);
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
void FenetrePrincipale::on_diametreSoleilValider_clicked() {
	emit diametreSoleil(ui->valeurDiametreSoleil->value());
	cout << "Diam modif : " << ui->valeurDiametreSoleil->value() << endl;
}

void FenetrePrincipale::statutCamera(int etat) {
	if(etat & CAMERA_CONNEXION_ON) {
		ui->statutCamera->setPalette(paletteTexteVert);
		ui->statutCamera->setText("Caméra connectée");
	}
	else if(etat & CAMERA_CONNEXION_OFF){
		ui->statutCamera->setPalette(paletteTexteRouge);
		ui->statutCamera->setText("Caméra déconnectée");
	}
}

void FenetrePrincipale::statutArduino(int etat) {
	switch(etat) {
		case ARDUINO_CONNEXION_ON:
			ui->statutArduino->setPalette(paletteTexteVert);
			ui->statutArduino->setText("Arduino connecté");
			break;
		case ARDUINO_CONNEXION_OFF:
			ui->statutArduino->setPalette(paletteTexteJaune);
			ui->statutArduino->setText("Arduino déconnecté");
			break;
		case ARDUINO_FICHIER_INTROUV:
			ui->statutArduino->setPalette(paletteTexteRouge);
			ui->statutArduino->setText("Arduino introuvable");
	}
}

void FenetrePrincipale::on_consigneHaut_clicked() {
	emit modificationConsigne(-1,0,modeVitesse);
}
void FenetrePrincipale::on_consigneBas_clicked() {
	emit modificationConsigne(1,0,modeVitesse);
}
void FenetrePrincipale::on_consigneDroite_clicked() {
	emit modificationConsigne(0,1,modeVitesse);
}
void FenetrePrincipale::on_consigneGauche_clicked() {
	emit modificationConsigne(0,-1,modeVitesse);
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

void FenetrePrincipale::modifierVitesseDeplacement() {
	if(ui->vitesseDecalageLent->isChecked()) {
		modeVitesse = VITESSE_LENTE;
	} else {
		modeVitesse = VITESSE_RAPIDE;
	}
}

void FenetrePrincipale::statutGuidage(bool statut) {
	if(statut) {
		ui->statutGuidage->setPalette(paletteTexteVert);
		ui->statutGuidage->setText("Marche");
	}
	else {
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt");
	}
}

void FenetrePrincipale::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
		case Qt::Key_Up:
			emit modificationConsigne(-1,0,modeVitesse);
			break;
		case Qt::Key_Down:
			emit modificationConsigne(1,0,modeVitesse);
			break;
		case Qt::Key_Right:
			emit modificationConsigne(0,1,modeVitesse);
			break;
		case Qt::Key_Left:
			emit modificationConsigne(0,-1,modeVitesse);
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
    		 || obj ==  ui->valeurDiametreSoleil
    		 || obj ==  ui->diametreSoleilValider
    		 || obj ==  ui->imageCamera
    		 || obj ==  ui->initialiserConsigne
    		 || obj ==  ui->lancerGuidage
    		 || obj ==  ui->messages
    		 || obj ==  ui->vitesseDecalageRapide
    		 || obj ==  ui->vitesseDecalageLent
     ) {
    	 if (event->type() == QEvent::KeyPress) {
    		 QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    		 if(keyEvent->key() == Qt::Key_Up
    				 || keyEvent->key() == Qt::Key_Right
    				 || keyEvent->key() == Qt::Key_Left
    				 || keyEvent->key() == Qt::Key_Down) {

    			 keyPressEvent(keyEvent);
    			 return true;
    		 }
    		 else {
    			 return false;
    		 }
         } else {
             return false;
         }
     } else {
         //passer l'événement à la classe parent
         return QMainWindow::eventFilter(obj, event);
     }
}
