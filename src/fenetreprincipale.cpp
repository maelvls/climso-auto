/*
 * Guidage.cpp
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 *
 *	Classe d'interaction utilisateur correspondant à la fenetre principale. Il s'agit de la
 *	partie graphique du logiciel. La classe WidgetImage (widgetimage.cpp) fait aussi partie
 *	des classes d'interaction utilisateur.
 *
 *  La classe communique avec les classes Capture (capture.cpp) et Guidage (guidage.cpp) grâce au
 *  système de signaux-slots. Par exemple, lorsqu'on appuie sur le bonton "Reset de consigne",
 *  le signal clicked() provenant du bouton ui->consigneReset est envoyé au slot consigneReset().
 *  Ici les noms sont idientiques.
 *
 *  Pour que la connexion entre le signal et le slot se fasse, on écrit :
 *  	QObject::connect(ui->consigneReset,SIGNAL(clicked()),guidage,SLOT(consigneReset()));
 *
 *	Toutes les fonctions de fenetreprincipale sont liées à de l'intéraction avec l'utilisateur.
 *	Tous les traitements se font dans Capture ou Guidage.
 */

#include "fenetreprincipale.h"
#include "fenetreprincipale_ui.h"
#include <QtCore/QTextCodec>

/*
 * ATTENTION, dans les constructeurs de Capture et Guidage on ne peut pas envoyer de signal car
 * l'appel du constructeur est effectué avant que la connexion soit effectuée
 */
FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    // Lancement de l'interface utilisateur (setupUi provient de fenetreprincipale_ui.h
	// généré à partir de fenetreprincipale.ui, qui permet de modifier la disposition)
	ui->setupUi(this);

    // Couleurs de texte
    paletteTexteVert.setColor(QPalette::WindowText, Qt::green);
    paletteTexteRouge.setColor(QPalette::WindowText, Qt::red);
    paletteTexteJaune.setColor(QPalette::WindowText, Qt::yellow);

    // Faire en sorte que les chaines statiques "C-strings" soient considérées comme UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    guidage = new Guidage; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    guidage->moveToThread(&threadGuidage);
    capture = new Capture; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    capture->moveToThread(&threadCapture);

    // Signaux-slots entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(envoiEtatArduino(enum EtatArduino)),this,SLOT(modifierStatutArduino(enum EtatArduino)));
    QObject::connect(guidage,SIGNAL(envoiEtatGuidage(enum EtatGuidage)),this,SLOT(modifierStatutGuidage(enum EtatGuidage)));
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));
    QObject::connect(this,SIGNAL(stopperGuidage()),guidage,SLOT(stopperGuidage()));
    QObject::connect(this,SIGNAL(modificationConsigne(int,int,bool)),guidage,SLOT(modifierConsigne(int,int,bool)));
    QObject::connect(ui->consigneReset,SIGNAL(clicked()),guidage,SLOT(consigneReset()));
    QObject::connect(guidage,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));
    QObject::connect(&threadGuidage,SIGNAL(finished()),guidage,SLOT(deleteLater()));

    // Signaux-slots entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(envoiEtatCamera(enum EtatCamera)),this,SLOT(modifierStatutCamera(enum EtatCamera)));
    QObject::connect(capture,SIGNAL(diametreSoleil(int)),ui->valeurDiametreSoleil,SLOT(setValue(int)));
    QObject::connect(ui->valeurDiametreSoleil,SIGNAL(valueChanged(int)),capture,SLOT(modifierDiametre(int)));
    QObject::connect(this,SIGNAL(initialiserCapture()),capture,SLOT(initialiserObjetCapture()));
    QObject::connect(&threadCapture,SIGNAL(finished()),capture,SLOT(deleteLater()));

    // Signaux-slots entre capture et guidage
    QObject::connect(capture,SIGNAL(resultats(Image*,double,double,int,double)),guidage, SLOT(traiterResultatsCapture(Image*,double,double,int,double)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));

    // Signaux-slots entre les éléments de l'interface
    QObject::connect(guidage,SIGNAL(imageSoleil(Image*)),ui->imageCamera,SLOT(afficherImageSoleil(Image*)));
    QObject::connect(guidage,SIGNAL(repereConsigne(float,float,float,enum EtatConsigne)),ui->imageCamera,SLOT(afficherRepereConsigne(float,float,float,enum EtatConsigne)));
    QObject::connect(guidage,SIGNAL(repereSoleil(float,float,float,enum EtatPosition)),ui->imageCamera,SLOT(afficherRepereSoleil(float,float,float,enum EtatPosition)));

    // Lancement des threads
    threadGuidage.start();
    threadCapture.start();
    emit initialiserCapture(); // En plus du constructeur Capture() on appelle initialiserCapture()
    // à cause du fait que les signaux ne peuvent pas être envoyés depuis un constructeur.

    // Le mode de décalage rapide est par défaut
    ui->vitesseDecalageRapide->setChecked(true);

    // Pour capturer les touches directionnelles du clavier pour controler la consigne,
    // on met un filtre sur toutes les parties de l'interface, et on redéfinit la méthode
    // eventFilter() (voir plus bas)
    ui->messages->installEventFilter(this);
    ui->consigneDroite->installEventFilter(this);
    ui->centralWidget->installEventFilter(this);
	ui->centralWidget->installEventFilter(this);
	ui->consigneBas->installEventFilter(this);
	ui->consigneDroite->installEventFilter(this);
	ui->consigneGauche->installEventFilter(this);
	ui->consigneHaut->installEventFilter(this);
	ui->valeurDiametreSoleil->installEventFilter(this);
	ui->imageCamera->installEventFilter(this);
	ui->consigneReset->installEventFilter(this);
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

void FenetrePrincipale::modifierStatutCamera(enum EtatCamera etat) {
	switch (etat) {
		case CAMERA_CONNEXION_ON:
			ui->statutCamera->setPalette(paletteTexteVert);
			ui->statutCamera->setText("Caméra connectée");
			break;
		case CAMERA_CONNEXION_OFF:
			ui->statutCamera->setPalette(paletteTexteRouge);
			ui->statutCamera->setText("Caméra déconnectée");
			break;
		default:
			break;
	}
}

void FenetrePrincipale::modifierStatutArduino(enum EtatArduino etat) {
	switch(etat) {
	case ARDUINO_CONNEXION_ON:
		ui->statutArduino->setPalette(paletteTexteVert);
		ui->statutArduino->setText("Arduino connecté");
		break;
	case ARDUINO_CONNEXION_OFF:
		ui->statutArduino->setPalette(paletteTexteRouge);
		ui->statutArduino->setText("Arduino déconnecté");
		break;
	case ARDUINO_FICHIER_INTROUV:
		ui->statutArduino->setPalette(paletteTexteRouge);
		ui->statutArduino->setText("Arduino introuvable");
	}
}

/**
 * Slot permettant de modifier l'affichage de l'état du guidage
 * @param statut True pour guidage en marche
 */
void FenetrePrincipale::modifierStatutGuidage(enum EtatGuidage statut) {
	switch (statut) {
	case GUIDAGE_MARCHE:
		ui->statutGuidage->setPalette(paletteTexteVert);
		ui->statutGuidage->setText("Guidage en marche");
		break;
	case GUIDAGE_BESOIN_POSITION:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Pas de position du soleil");
		break;
	case GUIDAGE_ARRET_BRUIT:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: bruit/signal trop fort");
		break;
	case GUIDAGE_ARRET_NORMAL:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: tout est normal");
		break;
	case GUIDAGE_ARRET_DIVERGE:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: guidage sans effet");
		break;
	case GUIDAGE_ARRET_PANNE:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: caméra ou arduino HS");
		break;

	default:
		break;
	}
}

/*
 * NOTE SUR LES METHODES on_elementInterface_clicked() (par exemple) :
 *
 * Ces méthodes permettent de s'affranchir de l'étape signal-slot,
 * la méthode on_consigneHaut_clicked est appelée à chaque fois que
 * le bouton consigneHaut est pressé.
 *
 * Il s'agit d'un "raccourci" pour éviter de passer par un
 * signal -> slot -> appel de fonction.
 */

void FenetrePrincipale::on_consigneHaut_clicked() {
	emit modificationConsigne(-1,0,(ui->vitesseDecalageLent->isChecked()));
}
void FenetrePrincipale::on_consigneBas_clicked() {
	emit modificationConsigne(1,0,(ui->vitesseDecalageLent->isChecked()));
}
void FenetrePrincipale::on_consigneDroite_clicked() {
	emit modificationConsigne(0,1,(ui->vitesseDecalageLent->isChecked()));
}
void FenetrePrincipale::on_consigneGauche_clicked() {
	emit modificationConsigne(0,-1,(ui->vitesseDecalageLent->isChecked()));
}
void FenetrePrincipale::on_lancerGuidage_clicked() {
    emit lancerGuidage();
}
void FenetrePrincipale::on_stopperGuidage_clicked() {
	emit stopperGuidage();
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

void FenetrePrincipale::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
		case Qt::Key_Up:
			emit modificationConsigne(-1,0,(ui->vitesseDecalageLent->isChecked()));
			break;
		case Qt::Key_Down:
			emit modificationConsigne(1,0,(ui->vitesseDecalageLent->isChecked()));
			break;
		case Qt::Key_Right:
			emit modificationConsigne(0,1,(ui->vitesseDecalageLent->isChecked()));
			break;
		case Qt::Key_Left:
			emit modificationConsigne(0,-1,(ui->vitesseDecalageLent->isChecked()));
			break;
	}
}


// Pour capturer les touches directionnelles du clavier pour controler la consigne
bool FenetrePrincipale::eventFilter(QObject *obj, QEvent *event)
 {
     if ((obj ==  ui->centralWidget
    		 || obj ==  ui->consigneBas
    		 || obj ==  ui->consigneDroite
    		 || obj ==  ui->consigneGauche
    		 || obj ==  ui->consigneHaut
    		 || obj ==  ui->valeurDiametreSoleil
    		 || obj ==  ui->imageCamera
    		 || obj ==  ui->consigneReset
    		 || obj ==  ui->lancerGuidage
    		 || obj ==  ui->messages
    		 || obj ==  ui->vitesseDecalageRapide
    		 || obj ==  ui->vitesseDecalageLent)
    		 && event->type() == QEvent::KeyPress) {
    		 QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    		 if(keyEvent->key() == Qt::Key_Up
    				 || keyEvent->key() == Qt::Key_Right
    				 || keyEvent->key() == Qt::Key_Left
    				 || keyEvent->key() == Qt::Key_Down) {

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
