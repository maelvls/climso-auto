#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QThread>
#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"
#include "arduino.h"
#include "guidage.h"

namespace Ui {
class FenetrePrincipale;
}

class FenetrePrincipale : public QMainWindow
{   Q_OBJECT
private:
    Ui::FenetrePrincipale *ui;
    CSBIGCam *cam;
    Arduino* arduino;
    Image* img;
    CSBIGImg *img_sbig;
    QImage *img_affichee;
    QThread *threadGuidage;

    void connecterCamera();
    void deconnecterCamera();
    void capturerImage();
    int cameraConnectee();
    void essaiAffichageImage();
    void afficherImage(Image* img);

    void connecterArduino();
    void deconnecterArduino();
    void envoyerImpulsion(int pin, int duree);
    void guidageAuto();
public:
    explicit FenetrePrincipale(QWidget *parent = 0);
    ~FenetrePrincipale();

private slots:
    void on_connecterCamera_clicked();
    void on_deconnecterCamera_clicked();
    void on_capturerImage_clicked();
    void on_connecterArduino_clicked();
    void on_deconnecterArduino_clicked();
    void on_lancerGuidage_clicked();
    void on_stopperGuidage_clicked();
public slots:
	void afficherMessage(QString msg);
    void guidageTermine();
signals:
	void envoyerMessage(QString msg);
};

#endif // FENETREPRINCIPALE_H
