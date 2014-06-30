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

    // Liens entre fenetreprincipale et capture
    QObject::connect(capture,SIGNAL(etatCamera(int)),this,SLOT(statutCamera(int)));
    QObject::connect(this,SIGNAL(connecterCamera()),capture,SLOT(connecterCameraAuto()));
    QObject::connect(this,SIGNAL(deconnecterCamera()),capture,SLOT(deconnecterCameraAuto()));
    QObject::connect(ui->diametreSoleil,SIGNAL(valueChanged(int)),capture,SLOT(modifierDiametre(int)));


    // Liens entre capture et guidage
    QObject::connect(capture,SIGNAL(resultats(Image*,double,double,int,double)),guidage, SLOT(traiterResultatsCapture(Image*,double,double,int,double)));
    QObject::connect(capture,SIGNAL(stopperGuidage()),guidage, SLOT(stopperGuidage()));
    QObject::connect(ui->nomFichierArduino,SIGNAL(textChanged(QString)),guidage, SLOT(connecterArduino(QString)));

    // Liens avec imageCamera (le widget)
    QObject::connect(guidage,SIGNAL(imageSoleil(Image*)),ui->imageCamera,SLOT(afficherImageSoleil(Image*)));
    QObject::connect(guidage,SIGNAL(repereSoleil(float,float,float,QColor)),ui->imageCamera,SLOT(afficherRepere(float,float,float,QColor)));


    ui->diametreSoleil->setValue(200);

    threadGuidage.start();
    threadCapture.start();

    ui->messages->installEventFilter(this);
    ui->consigneDroite->installEventFilter(this);

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
	emit deconnecterArduino();
	emit deconnecterCamera();
	capture->deleteLater();
	guidage->deleteLater();
	cout << "Guidage termine" << endl;
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



bool FenetrePrincipale::eventFilter(QObject *obj, QEvent *event)
 {
     if (obj == ui->capturerImage
    		 || obj ==  ui->centralWidget
    		 || obj ==  ui->connecterArduino
    		 || obj ==  ui->consigneBas
    		 || obj ==  ui->consigneDroite
    		 || obj ==  ui->consigneGauche
    		 || obj ==  ui->consigneHaut
    		 || obj ==  ui->deconnecterArduino
    		 || obj ==  ui->deconnecterCamera
    		 || obj ==  ui->diametreSoleil
    		 || obj ==  ui->imageCamera
    		 || obj ==  ui->initialiserConsigne
    		 || obj ==  ui->lancerGuidage
    		 || obj ==  ui->messages
    		 || obj ==  ui->nomFichierArduino
    		 ) {
         if (event->type() == QEvent::KeyPress) {
             QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
             cout << "Touche clavier: " << keyEvent->key() <<endl;
             return true;
         } else {
             return false;
         }
     } else {
         //passer l'événement à la classe parent
         return QMainWindow::eventFilter(obj, event);
     }
}
