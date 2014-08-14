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
	param.setValue("arret-si-eloigne", ui->arretSiDivergenceOn->isChecked());
	param.setValue("duree-attente-avant-arret", ui->dureeAvantArretSiIncoherence->value());
	param.setValue("seuil-signal-bruit", ui->seuilSignalBruit->value());
	param.setValue("normaliser-image-affichee",ui->normaliser->isChecked());
}

void Parametres::chargerParametres() {
	QSettings param("irap", "climso-auto");
	ui->arretSiDivergenceOn->setChecked(param.value("arret-si-eloigne",true).toBool());
	ui->dureeAvantArretSiIncoherence->setValue(param.value("duree-attente-avant-arret",2).toInt());
	ui->seuilSignalBruit->setValue(param.value("seuil-signal-bruit",10).toDouble());
	ui->normaliser->setChecked(param.value("normaliser-image-affichee",true).toBool());
}

