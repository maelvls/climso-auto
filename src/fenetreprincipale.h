#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"

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

private:
    Ui::FenetrePrincipale *ui;
    CSBIGCam *cam;
    Image* img;
    CSBIGImg *img_sbig;
    QImage *img_affichee;

    void connecterCamera();
    void deconnecterCamera();
    void capturerImage();
    void afficherMessage(string err);
    int cameraConnectee();
    void essaiAffichageImage();
};

#endif // FENETREPRINCIPALE_H
