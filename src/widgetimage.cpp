/*
 * widgetImage.cpp
 *
 *  Created on: 19 juin 2014
 *      Author: admin
 */

#include "widgetimage.h"
#include <iostream>

/*
void WidgetImage::paintEvent(QPaintEvent *e) // redéfinition de paintEvent()
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::darkGreen);
	painter.drawRect(0, 0, this->size().width(), this->size().height());
	painter.drawLine(0, this->size().height()/2, this->size().width(), this->size().height()/2);
}
*/

WidgetImage::WidgetImage(QWidget* parent):QLabel(parent) {}

WidgetImage::~WidgetImage() {
}

void WidgetImage::afficherImage(Image* img) {
	unsigned char *img_uchar = img->versUchar();
	this->setAutoFillBackground(true);
	// Creation de l'index (34 va donner 34...) car Qt ne gère pas les nuances de gris
	QImage *temp = new QImage(img_uchar, img->getColonnes(), img->getLignes(),img->getColonnes(), QImage::Format_Indexed8);
	for(int i=0;i<256;++i) { // Pour construire une image en nuances de gris (n'existe pas sinon sous Qt)
		temp->setColor(i, qRgb(i,i,i));
	}

	QImage image = temp->scaled(this->width(),this->height(),Qt::KeepAspectRatio);
	rawImage = QPixmap::fromImage(image,Qt::AutoColor);
	delete temp;
	// FIXME il faut delete temp

	this->setPixmap(rawImage); // On affiche
}

void WidgetImage::afficherPointeur(int x, int y) {
}

void WidgetImage::afficherCercle(float x_pourcent, float y_pourcent, float diametre_pourcent_x) {
	QPainter p(&rawImage);
	p.setPen(Qt::yellow);

	int position_x = rawImage.width() * x_pourcent;
	int position_y = rawImage.height() * y_pourcent;
	int diametre = rawImage.width() * diametre_pourcent_x;
	QPoint centre(position_x,position_y);

	p.drawEllipse(centre,diametre/2,diametre/2);
	this->setPixmap(rawImage); // On affiche
}
