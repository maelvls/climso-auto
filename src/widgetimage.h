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

#include <QtGui/qlabel.h>
#include <QtGui/qpainter.h>
#include "image.h"

class WidgetImage: public QLabel {
	Q_OBJECT

	QPixmap rawImage;
    //void paintEvent(QPaintEvent *e);
private:

public:
	WidgetImage(QWidget *parent = 0);
	virtual ~WidgetImage();
public slots:
	void afficherImage(Image* img);
	void afficherPointeur(int x, int y);
	void afficherCercle(int x, int y, int diametre);
};

#endif /* WIDGETIMAGE_H_ */
