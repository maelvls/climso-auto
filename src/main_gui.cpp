/*
 *  main_qt_gui.cpp
 *  climso-auto
 *
 *  Created by Maël Valais on 10/06/2014.
 *
 *  Permet de lancer l'environnement de la fenetre principale.
 *  La fonction exec() lance la boucle d'events de la QApplication.
 */

#include "fenetreprincipale.h"
#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FenetrePrincipale w;
    w.show();
    return a.exec();
}
