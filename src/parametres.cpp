/*
 * parametres.cpp
 *
 *  Created on: 8 juil. 2014
 *      Author: Maël Valais
 *
 *  Boite de parametres. Je choisis d'appliquer les parametres quand on appuie sur OK :
 *  http://www.developpez.net/forums/d919175/c-cpp/bibliotheques/qt/boite-parametre-d-application/#post5194027
 *  Quand l'appui sera fait, on sauvegardera à travers QSettings ; on enverra ensuite des signaux à Guidage
 *  et Capture pour qu'ils mettent à jour leurs parametres (attention, il faut aussi qu'ils sauvegardent les
 *  parametres en cours d'utilisation)
 */

#include "parametres.h"
#include "parametres_ui.h"
#include "guidage.h"
#include "capture.h"
#include <QtCore/QSettings>

Parametres::Parametres(QWidget* parent):QDialog(parent),ui(new Ui::Parametres)
{
	ui->setupUi(this);
}



Parametres::~Parametres()
{
	enregistrerParametres();
}

void Parametres::enregistrerParametres() {
	QSettings param("irap", "climso-auto");
	param.setValue("orient-nord-sud-inversee",  ui->orientationVerticaleInverse->isChecked());
	param.setValue("orient-est-ouest-inversee", ui->orientationHorizontaleInverse->isChecked());
}

void Parametres::chargerParametres() {
	QSettings param("irap", "climso-auto");
	ui->orientationVerticaleInverse->setChecked(param.value("orient-nord-sud-inversee",false).toBool());
	ui->orientationHorizontaleInverse->setChecked(param.value("orient-est-ouest-inversee",false).toBool());
}
