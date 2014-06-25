/*
 * widgetImage.cpp
 *
 *  Created on: 19 juin 2014
 *      Author: admin
 */

#include "widgetimage.h"
#include <iostream>

WidgetImage::WidgetImage(QWidget* parent):QLabel(parent) {
}

WidgetImage::~WidgetImage() {
}

void WidgetImage::afficherImageSoleil(Image* imgOrig) {
	Image* img = new Image(*imgOrig);
	img->normaliser();
	unsigned char *img_uchar = img->versUchar();
	this->setAutoFillBackground(true);
	// Creation de l'index (34 va donner 34...) car Qt ne gère pas les nuances de gris
	QImage *temp = new QImage(img_uchar, img->getColonnes(), img->getLignes(),img->getColonnes(), QImage::Format_Indexed8);
	for(int i=0;i<256;++i) { // Pour construire une image en nuances de gris (n'existe pas sinon sous Qt)
		temp->setColor(i, qRgb(i,i,i));
	}
	QImage image = temp->scaled(this->width(),this->height(),Qt::KeepAspectRatio); // FIXME fuite mémoire ???
	rawImage = QPixmap::fromImage(image,Qt::AutoColor);
	this->setPixmap(rawImage); // On affiche

	delete img;
	delete temp;

}

void WidgetImage::afficherRepereConsigne(float x_pourcent, float y_pourcent, float diametre_pourcent_x) {
	QPainter p(&rawImage);
	p.setPen(Qt::yellow);
	int position_x = rawImage.width() * x_pourcent;
	int position_y = rawImage.height() * y_pourcent;
	int diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	p.drawEllipse(centre,diametre/2,diametre/2);
	this->setPixmap(rawImage); // On affiche
}

void WidgetImage::afficherRepereCourant(float x_pourcent, float y_pourcent, float diametre_pourcent_x) {
	QPainter p(&rawImage);
	p.setPen(Qt::gray);
	int position_x = rawImage.width() * x_pourcent;
	int position_y = rawImage.height() * y_pourcent;
	int diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	p.drawEllipse(centre,diametre/2,diametre/2);
	this->setPixmap(rawImage); // On affiche
}
