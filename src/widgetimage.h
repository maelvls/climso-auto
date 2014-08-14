/*
 * widgetImage.h
 *
 *  Created on: 19 juin 2014
 *      Author: mael valais
 *
 *  Cette classe permet l'affichage d'une image dans un widget QLabel
 *
 *  NOTE QTDESIGNER: pour utiliser cette classe dérivée à partir de QtDesigner (ou QtCreator)
 *  il faut sélectionner le widget (ici, il s'appelait imageSoleil) et faire Promote to...
 *  et choisir la classe widgetimage.h
 */

#ifndef WIDGETIMAGE_H_
#define WIDGETIMAGE_H_

#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include "guidage.h"

class WidgetImage: public QLabel {
	Q_OBJECT
	QPixmap rawImage;
private:

public:
	WidgetImage(QWidget *parent = 0);
	virtual ~WidgetImage();
public slots:
	void afficherImageSoleil(QImage img);
	void afficherRepereCourant(float pourcent_x, float pourcent_y, float diametre_pourcent_x, EtatPosition);
	void afficherRepereConsigne(float pourcent_x, float pourcent_y, float diametre_pourcent_x, EtatConsigne);
};

#endif /* WIDGETIMAGE_H_ */
