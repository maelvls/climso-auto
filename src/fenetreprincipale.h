#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QtGui/QMainWindow>
#include <QtCore/QThread>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"
#include "arduino.h"

#include "guidage.h"
#include "capture.h"

#include <signal.h>

namespace Ui {
class FenetrePrincipale;
}

class FenetrePrincipale : public QMainWindow
{   Q_OBJECT
private:
    Ui::FenetrePrincipale *ui;
    QThread threadGuidage;
    Guidage* guidage;
    QThread threadCapture;
    Capture* capture;

    QPalette paletteTexteVert;
    QPalette paletteTexteRouge;
    QPalette paletteTexteJaune;
    QPalette paletteTexteGris;

public:
    explicit FenetrePrincipale(QWidget *parent = 0);
    ~FenetrePrincipale();

private slots:
    void on_lancerGuidage_clicked();
    void on_stopperGuidage_clicked();
    void on_consigneHaut_clicked();
    void on_consigneBas_clicked();
    void on_consigneDroite_clicked();
    void on_consigneGauche_clicked();
    void on_ouvrirParametres_triggered();
    void on_afficherPositionConsigne_clicked();
    void on_afficherPositionCourante_clicked();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
	void afficherMessage(QString msg);
    void modifierStatutCamera(EtatCamera);
    void modifierStatutArduino(EtatArduino);
    void modifierStatutGuidage(EtatGuidage);
    void modifierStatutPosition(EtatPosition);
    void modifierStatutConsigne(EtatConsigne);
    void afficherPositionConsigne(double x, double y);
    void afficherPositionCourante(double x, double y);
signals:
	void lancerGuidage();
	void stopperGuidage();
	void modificationConsigne(int,int,bool decalageLent);
	void quitter();
	void diametreSoleil(int);
	void demanderEnregistrementParametres();
	void demanderChargementParametres();
	void changerAffichageRepereConsigne(bool);
	void changerAffichageRepereCourant(bool);
};

#endif // FENETREPRINCIPALE_H
