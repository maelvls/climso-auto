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
    QImage *img_affichee;
    QThread threadGuidage;
    Guidage* guidage;


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
signals:
	void lancerGuidage();
	void connecterCamera();
	void deconnecterCamera();
	void capturerImage();
	void connecterArduino(QString);
	void deconnecterArduino();
};

#endif // FENETREPRINCIPALE_H
