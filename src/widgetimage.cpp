/*
 * widgetImage.cpp
 *
 *  Created on: 19 juin 2014
 *      Author: admin
 */

#include "widgetimage.h"
#include <iostream>
#include <QtGui/QPen>

WidgetImage::WidgetImage(QWidget* parent):QLabel(parent) {
}

WidgetImage::~WidgetImage() {
}

void WidgetImage::afficherImageSoleil(Image* imgOrig) {
	Image* img = new Image(*imgOrig);
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

void WidgetImage::afficherRepereConsigne(float x_pourcent, float y_pourcent, float diametre_pourcent_x,QColor couleur) {
	QPainter paint(&rawImage);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(couleur);
	paint.setPen(pen);
	int position_x = rawImage.width() * x_pourcent;
	int position_y = rawImage.height() * y_pourcent;
	int diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	paint.drawEllipse(centre,diametre/2,diametre/2);
	paint.drawLine(position_x-10,position_y,position_x+10,position_y);
	paint.drawLine(position_x,position_y-10,position_x,position_y+10);
	//QPoint decalageDepuisCentre(diametre/2,diametre/2);
	//paint.drawText(centre - decalageDepuisCentre,"Position de consigne");
	paint.drawText(10,40,"Position de consigne");
	this->setPixmap(rawImage); // On affiche
}
void WidgetImage::afficherRepereSoleil(float x_pourcent, float y_pourcent, float diametre_pourcent_x,QColor couleur) {
	QPainter paint(&rawImage);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(couleur);
	pen.setWidth(1);
	paint.setPen(pen);
	int position_x = rawImage.width() * x_pourcent;
	int position_y = rawImage.height() * y_pourcent;
	int diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);
	paint.drawEllipse(centre,diametre/2,diametre/2);
	paint.drawLine(position_x-10,position_y,position_x+10,position_y);
	paint.drawLine(position_x,position_y-10,position_x,position_y+10);
	//QPoint decalageDepuisCentre(diametre/2-20,diametre/2-20);
	//paint.drawText(centre + decalageDepuisCentre,"Position reelle");
	paint.drawText(10,20,"Position reelle");
	this->setPixmap(rawImage); // On affiche
}
