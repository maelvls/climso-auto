/*
 * parametres.h
 *
 *  Created on: 8 juil. 2014
 *      Author: Maël Valais
 */

#ifndef PARAMETRES_H_
#define PARAMETRES_H_

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QDialog>
#else
#include <QtGui/QDialog>
#endif

namespace Ui {
	class Parametres;
}

class Parametres : public QDialog {
	Q_OBJECT
public:
	Parametres(QWidget* parent);
	virtual ~Parametres();
private:
	Ui::Parametres* ui;
public slots:
	void enregistrerParametres();
	void chargerParametres();
};

#endif /* PARAMETRES_H_ */
