/*
 * guidage.h
 *
 *  Created on: 16 juin 2014
 *      Author: Maël Valais
 *
 *  Cette classe contient les slots et signaux permettant de lancer l'execution de la boucle
 *  de guidage
 */

#ifndef GUIDAGE_H_
#define GUIDAGE_H_

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include "arduino.h"
#include "image.h"



class Guidage : public QObject {
	Q_OBJECT
private:
	QTimer timerCorrection;
	QTimer timerVerificationConnexions;
	QThread threadDuGuidage;
	Arduino* arduino;
	QString fichier_arduino;
	double consigne_l, consigne_c;
	double position_l, position_c;
	int l_max, c_max; // Pour l'affichage
	int diametre; // Pour l'affichage

	void capturerImage();
    bool arduinoConnecte();
public:
	Guidage();
public slots:
// guidage
	void lancerGuidage();
	void stopperGuidage();
// arduino
    void connecterArduino(QString nom);
    void deconnecterArduino();
	void envoyerCmd(int pin, int duree);
private slots:
	void guidageSuivant();
	void guidageInitial();
	void connexionAuto();
	void modifierPosition(double l, double c, int l_max, int c_max, int diametre);
	void consigneModifier(int deltaLigne, int deltaColonne);
signals:
	void repereSoleil(float pourcent_x, float pourcent_y, float diametre_pourcent_x);
	void message(QString msg);
	void etatArduino(bool);
	void etatGuidage(bool);
};

#endif /* GUIDAGE_H_ */
