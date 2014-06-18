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
    QThread threadGuidage;
    Guidage* guidage;

    QPalette paletteOk;
    QPalette palettePasOk;


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
    void afficherImage(Image* img);
    void guidageTermine();
    void statutCamera(bool etat);
    void statutArduino(bool etat);
    void afficherConsigne(double l, double c);
signals:
	void lancerGuidage(bool);
	void connecterCamera();
	void deconnecterCamera();
	void demanderImage();
	void connecterArduino(QString);
	void deconnecterArduino();
	void consigneLigne(double l);
	void consigneColonne(double c);
};

#endif // FENETREPRINCIPALE_H
