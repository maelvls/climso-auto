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
#include "parametres.h"
#include <QtCore/QTextCodec>

/*
 * ATTENTION, dans les constructeurs de Capture et Guidage on ne peut pas envoyer de signal car
 * l'appel du constructeur est effectué avant que la connexion soit effectuée.
 * POUR CAPTURE: au démarrage, envoie le diamètre qu'il a lu dans les paramètres à FenetrePrincipale.
 * J'ai donc choisi d'envoyer le chargement des paramètres après avoir fait start().
 * POUR PARAMETRES: on appelle chargerParametres() avant de faire exec(), c'est possible car
 * l'objet est dans le même thread.
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
    paletteTexteGris.setColor(QPalette::WindowText, Qt::gray);

    // Faire en sorte que les chaines statiques "C-strings" soient considérées comme UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    guidage = new Guidage; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    guidage->moveToThread(&threadGuidage);
    capture = new Capture; // ATTENTION: pas d'envoi de signaux dans le constructeur !
    capture->moveToThread(&threadCapture);

    // Signaux-slots entre fenetreprincipale et guidage
    QObject::connect(guidage,SIGNAL(envoiEtatArduino(EtatArduino)),this,SLOT(modifierStatutArduino(EtatArduino)));
    QObject::connect(guidage,SIGNAL(envoiEtatGuidage(EtatGuidage)),this,SLOT(modifierStatutGuidage(EtatGuidage)));
    QObject::connect(guidage,SIGNAL(envoiEtatPosition(EtatPosition)),this,SLOT(modifierStatutPosition(EtatPosition)));
    QObject::connect(guidage,SIGNAL(envoiEtatConsigne(EtatConsigne)),this,SLOT(modifierStatutConsigne(EtatConsigne)));
    QObject::connect(guidage,SIGNAL(envoiPositionCourante(double,double)),this,SLOT(afficherPositionCourante(double,double)));
    QObject::connect(guidage,SIGNAL(envoiPositionConsigne(double,double)),this,SLOT(afficherPositionConsigne(double,double)));
    QObject::connect(guidage,SIGNAL(message(QString)),this,SLOT(afficherMessage(QString)));
    QObject::connect(this,SIGNAL(lancerGuidage()),guidage,SLOT(lancerGuidage()));
    QObject::connect(this,SIGNAL(stopperGuidage()),guidage,SLOT(stopperGuidage()));
    QObject::connect(this,SIGNAL(modificationConsigne(int,int,bool)),guidage,SLOT(modifierConsigne(int,int,bool)));
    QObject::connect(ui->consigneReset,SIGNAL(clicked()),guidage,SLOT(consigneReset()));
    QObject::connect(guidage,SIGNAL(signalBruit(double)),ui->ratioSignalBruit,SLOT(setNum(double)));
    QObject::connect(&threadGuidage,SIGNAL(finished()),guidage,SLOT(deleteLater()));
    QObject::connect(this,SIGNAL(demanderEnregistrementParametres()),guidage,SLOT(enregistrerParametres()));
    QObject::connect(this,SIGNAL(demanderChargementParametres()),guidage,SLOT(chargerParametres()));

    // Signaux-slots entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(envoiEtatCamera(EtatCamera)),this,SLOT(modifierStatutCamera(EtatCamera)));
    QObject::connect(capture,SIGNAL(diametreSoleil(int)),ui->valeurDiametreSoleil,SLOT(setValue(int)));
    QObject::connect(ui->valeurDiametreSoleil,SIGNAL(valueChanged(int)),capture,SLOT(modifierDiametre(int)));
    QObject::connect(&threadCapture,SIGNAL(finished()),capture,SLOT(deleteLater()));
    QObject::connect(this,SIGNAL(demanderEnregistrementParametres()),capture,SLOT(enregistrerParametres()));
    QObject::connect(this,SIGNAL(demanderChargementParametres()),capture,SLOT(chargerParametres()));
    QObject::connect(ui->chercherDiametre,SIGNAL(clicked()),capture,SLOT(chercherDiametreProche()));
    QObject::connect(this,SIGNAL(changerAffichageRepereConsigne(bool)),guidage,SLOT(afficherRepereConsigne(bool)));
    QObject::connect(this,SIGNAL(changerAffichageRepereCourant(bool)),guidage,SLOT(afficherRepereCourant(bool)));


    // Signaux-slots entre capture et guidage
    QObject::connect(capture,SIGNAL(resultats(QImage,double,double,int,double)),guidage, SLOT(traiterResultatsCapture(QImage,double,double,int,double)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));

    // Signaux-slots entre les éléments de l'interface
    QObject::connect(guidage,SIGNAL(imageSoleil(QImage)),ui->imageCamera,SLOT(afficherImageSoleil(QImage)));
    QObject::connect(guidage,SIGNAL(repereConsigne(float,float,float,EtatConsigne)),ui->imageCamera,SLOT(afficherRepereConsigne(float,float,float,EtatConsigne)));
    QObject::connect(guidage,SIGNAL(repereCourant(float,float,float,EtatPosition)),ui->imageCamera,SLOT(afficherRepereCourant(float,float,float,EtatPosition)));



    // Lancement des threads
    threadGuidage.start();
    threadCapture.start();
    emit demanderChargementParametres(); // Comme le chargement implique des signaux, il faut charger
    // après le début des threads

    // Le mode de décalage rapide est par défaut
    ui->vitesseDecalageRapide->setChecked(false);

    etatBoutonMarche = true; // True pour "Marche", false pour "Arrêt"

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

	std::cout << "Programme lancé" << endl;
}

FenetrePrincipale::~FenetrePrincipale() {
    delete ui;
}

void FenetrePrincipale::afficherMessage(QString msg) {
    ui->messages->append(msg);
}

void FenetrePrincipale::modifierStatutCamera(EtatCamera etat) {

	switch (etat) {
		case CAMERA_CONNEXION_ON:
			ui->statutCamera->setPalette(paletteTexteVert);
			ui->statutCamera->setText("Caméra connectée");
			ui->cadreGestionGuidage->setEnabled(true);
			break;
		case CAMERA_CONNEXION_OFF:
			ui->statutCamera->setPalette(paletteTexteRouge);
			ui->statutCamera->setText("Caméra déconnectée");
			ui->cadreGestionGuidage->setEnabled(false);
			break;
		default:
			break;
	}
}

void FenetrePrincipale::modifierStatutArduino(EtatArduino etat) {
	switch(etat) {
	case ARDUINO_CONNEXION_ON:
		ui->statutArduino->setPalette(paletteTexteVert);
		ui->statutArduino->setText("Arduino connecté");
		ui->cadreGestionGuidage->setEnabled(true);
		break;
	case ARDUINO_CONNEXION_OFF:
		ui->statutArduino->setPalette(paletteTexteRouge);
		ui->statutArduino->setText("Arduino déconnecté");
		ui->cadreGestionGuidage->setEnabled(false);
		break;
	case ARDUINO_FICHIER_INTROUV:
		ui->statutArduino->setPalette(paletteTexteRouge);
		ui->statutArduino->setText("Arduino introuvable");
		ui->cadreGestionGuidage->setEnabled(false);
	}
}

/**
 * Slot permettant de modifier l'affichage de l'état du guidage
 * @param statut True pour guidage en marche
 */
void FenetrePrincipale::modifierStatutGuidage(EtatGuidage statut) {
	ui->instructionsGuidage->setText("");
	ui->cadreResetConsigne->setEnabled(true);
	ui->cadreDeplacementConsigne->setEnabled(false);
	ui->lancerGuidage->setText("Lancer le guidage");
	switch (statut) {
	case GUIDAGE_MARCHE:
		ui->statutGuidage->setPalette(paletteTexteVert);
		ui->statutGuidage->setText("Guidage en marche");
		ui->cadreResetConsigne->setEnabled(false);
		ui->cadreDeplacementConsigne->setEnabled(true);
		ui->instructionsGuidage->setText("L'autoguidage est en marche, il ne faut pas utiliser le joystick. Pour bouger le Soleil, déplacez la consigne.");
		ui->lancerGuidage->setText("Arrêter le guidage");
		break;
	case GUIDAGE_MARCHE_MAIS_BRUIT:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Pause: signal/bruit trop faible");
		ui->instructionsGuidage->setText("Le soleil est trop peu visible. L'autoguidage attend que le signal/bruit remonte.");
		break;
	case GUIDAGE_BESOIN_POSITION:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: Pas de position du soleil");
		ui->instructionsGuidage->setText("Aucune image n'a pu être effectuée, et donc aucune position courante n'est disponible.");
		break;
	case GUIDAGE_ARRET_BRUIT:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: signal/bruit trop faible");
		ui->instructionsGuidage->setText("Le soleil était trop peu visible pendant trop longtemps (durée réglable dans menu > paramètres)");
		break;
	case GUIDAGE_ARRET_NORMAL:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: tout est normal");
		ui->instructionsGuidage->setText("Pour démarrer : utilisez le joystick pour centrer le Soleil sur C1 puis appuyez sur 'Nouvelle pos. de consigne' et enfin sur 'Marche'.");
		break;
	case GUIDAGE_ARRET_DIVERGE:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: guidage sans effet");
		break;
	case GUIDAGE_ARRET_PANNE:
		ui->statutGuidage->setPalette(paletteTexteRouge);
		ui->statutGuidage->setText("Arrêt: caméra ou arduino off");
		break;

	default:
		break;
	}
}

void FenetrePrincipale::modifierStatutPosition(EtatPosition statut) {
	switch (statut) {
	case POSITION_COHERANTE:
		ui->ratioSignalBruit->setPalette(paletteTexteVert);
		ui->widgetPositionCourante->setPalette(paletteTexteVert);
		ui->instructionsSignalBruit->setText("Le Soleil est suffisemment visible, la position courante est fiable.");
		ui->textePositionCourante->setText("Position courante du Soleil");
		ui->textePositionCourante->setPalette(paletteTexteVert);
		break;
	case POSITION_INCOHERANTE:
		ui->ratioSignalBruit->setPalette(paletteTexteRouge);
		ui->widgetPositionCourante->setPalette(paletteTexteRouge);
		ui->instructionsSignalBruit->setText("Le Soleil est peu visible, la position courante n'est pas fiable.");
		ui->textePositionCourante->setText("Position courante du Soleil introuvable");
		ui->textePositionCourante->setPalette(paletteTexteRouge);
		break;
	default:
		break;
	}
}

void FenetrePrincipale::modifierStatutConsigne(EtatConsigne statut) {
	switch (statut) {
	case CONSIGNE_OK:
		ui->widgetPositionConsigne->setPalette(paletteTexteJaune);
		break;
	case CONSIGNE_LOIN:
		ui->widgetPositionConsigne->setPalette(paletteTexteJaune);
		break;
	default:
		ui->widgetPositionConsigne->setPalette(paletteTexteJaune);
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
    if(etatBoutonMarche == true) {
    	etatBoutonMarche = false;
    	emit stopperGuidage();
    }
    else {
    	etatBoutonMarche = true;
		emit lancerGuidage();
    }
}

void FenetrePrincipale::on_ouvrirParametres_triggered() {
	emit demanderEnregistrementParametres();
	Parametres fenetreParametres(this);
	fenetreParametres.chargerParametres(); // Cette méthode doit être lancée hors du constructeur
	fenetreParametres.exec();
	emit demanderChargementParametres();
}
void FenetrePrincipale::on_afficherPositionConsigne_clicked() {
	static bool etatBouton = true; // Initialisée qu'au lancement de la méthode
	if(etatBouton) { // Si le bouton affichait "On"
		ui->afficherPositionConsigne->setText("Off");
		emit changerAffichageRepereConsigne(etatBouton = false);
	} else { // Si le bouton affichait "Off"
		ui->afficherPositionConsigne->setText("On");
		emit changerAffichageRepereConsigne(etatBouton = true);
	}
}
void FenetrePrincipale::on_afficherPositionCourante_clicked() {
	static bool etatBouton = true; // Initialisée qu'au lancement de la méthode
	if(etatBouton) { // Si le bouton affichait "On"
		ui->afficherPositionCourante->setText("Off");
		emit changerAffichageRepereCourant(etatBouton = false);
	} else { // Si le bouton affichait "Off"
		ui->afficherPositionCourante->setText("On");
		emit changerAffichageRepereCourant(etatBouton = true);
	}
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

void FenetrePrincipale::afficherPositionConsigne(double x, double y) {
	ui->positionConsigneX->setText("x="+QString::number(x));
	ui->positionConsigneY->setText("y="+QString::number(y));
}

void FenetrePrincipale::afficherPositionCourante(double x, double y) {
	ui->positionCouranteX->setText("x="+QString::number(x));
	ui->positionCouranteY->setText("y="+QString::number(y));}
