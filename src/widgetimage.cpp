/*
 * widgetImage.cpp
 *
 *  Created on: 19 juin 2014
 *      Author: Maël Valais
 *
 *
 *	Classe d'interaction utilisateur correspondant au cadre contenant l'image du soleil.
 *	L'instance de la classe (on appelle cela un widget) est dans le même thread que
 *	FenetrePrincipale.
 *
 *  Cette classe gère:
 *  	- le passage des Image au format d'image accepté par Qt et leur affichage,
 *  	- l'affichage d'un repère (sorte de cible) permettant de visualiser les positions.
 */

#include "widgetimage.h"
#include <iostream>
#include <QtGui/QPen>

WidgetImage::WidgetImage(QWidget* parent):QLabel(parent) {
}

WidgetImage::~WidgetImage() {
}

void WidgetImage::afficherImageSoleil(QImage img) {
	QImage image = img.scaled(this->width(),this->height(),Qt::KeepAspectRatio); // FIXME fuite mémoire ???
	rawImage = QPixmap::fromImage(image,Qt::AutoColor);
	this->setPixmap(rawImage); // On affiche
	this->setAutoFillBackground(true);
	// FIXME: faut il delete des trucs ?
}

void WidgetImage::afficherRepereConsigne(float x_pourcent, float y_pourcent, float diametre_pourcent_x,EtatConsigne etatConsigne) {
	QColor couleur;
	switch (etatConsigne) {
	case CONSIGNE_OK: couleur = Qt::yellow; break;
	case CONSIGNE_LOIN: couleur = Qt::yellow; break;
	case CONSIGNE_DIVERGE: couleur = Qt::red; break;
	case CONSIGNE_NON_INITIALISEE: return; break;
	default: couleur = Qt::red; break;
	}
	QPainter paint(&rawImage);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(couleur);
	paint.setPen(pen);
	float position_x = rawImage.width() * x_pourcent;
	float position_y = rawImage.height() * y_pourcent;
	float diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	paint.drawEllipse(centre,(int)diametre/2,(int)diametre/2);
	paint.drawLine(position_x-10,position_y,position_x+10,position_y);
	paint.drawLine(position_x,position_y-10,position_x,position_y+10);
	this->setPixmap(rawImage); // On affiche
}
void WidgetImage::afficherRepereCourant(float x_pourcent, float y_pourcent, float diametre_pourcent_x, EtatPosition etatPosition) {
	QColor couleur;
	switch (etatPosition) {
	case POSITION_COHERANTE: couleur = Qt::green; break;
	case POSITION_INCOHERANTE: couleur = Qt::gray; break;
	case POSITION_NON_INITIALISEE: return;
	default: couleur = Qt::gray; break;
	}

	QPainter paint(&rawImage);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(couleur);
	pen.setWidth(1);
	paint.setPen(pen);
	float position_x = rawImage.width() * x_pourcent;
	float position_y = rawImage.height() * y_pourcent;
	float diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	paint.drawEllipse(centre,(int)diametre/2,(int)diametre/2);
	paint.drawLine(position_x-10,position_y,position_x+10,position_y);
	paint.drawLine(position_x,position_y-10,position_x,position_y+10);
	//QPoint decalageDepuisCentre(diametre/2-20,diametre/2-20);
	//paint.drawText(centre + decalageDepuisCentre,"Position reelle");
	//paint.drawText(10,20,"Position reelle");
	//paint.drawText(rawImage.width()-165,20,"x="+QString::number(position_x)+", y="+QString::number(position_y)+"");
	this->setPixmap(rawImage); // On affiche
}
