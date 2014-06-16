#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"
#include "arduino.h"

namespace Ui {
class FenetrePrincipale;
}

class FenetrePrincipale : public QMainWindow
{   Q_OBJECT
public:
    explicit FenetrePrincipale(QWidget *parent = 0);
    ~FenetrePrincipale();
private slots:
    void on_connecterCamera_clicked();

    void on_deconnecterCamera_clicked();

    void on_capturerImage_clicked();

    void on_connecterArduino_clicked();

    void on_deconnecterArduino_clicked();

private:
    Ui::FenetrePrincipale *ui;
    CSBIGCam *cam;
    Arduino* arduino;
    Image* img;
    CSBIGImg *img_sbig;
    QImage *img_affichee;

    void afficherMessage(string err);


    void connecterCamera();
    void deconnecterCamera();
    void capturerImage();
    int cameraConnectee();
    void essaiAffichageImage();

    void connecterArduino();
    void deconnecterArduino();
    void envoyerImpulsion(int pin, int duree);
    void afficherMessage(QString err);
};

#endif // FENETREPRINCIPALE_H
