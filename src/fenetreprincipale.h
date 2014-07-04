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

    int modeVitesse; // VITESSE_LENTE ou VITESSE_RAPIDE

    QPalette paletteTexteVert;
    QPalette paletteTexteRouge;
    QPalette paletteTexteJaune;

    void signalHandler(int signal);

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

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);
    void modifierVitesseDeplacement();

public slots:
	void afficherMessage(QString msg);
    void statutCamera(int etat);
    void statutArduino(int etat);
    void statutGuidage(bool statut);

signals:
	void lancerGuidage();
	void stopperGuidage();
	void modificationConsigne(int,int,int modeVitesse);
	void quitter();
	void diametreSoleil(int);
	void initialiserCapture();
};

#endif // FENETREPRINCIPALE_H
