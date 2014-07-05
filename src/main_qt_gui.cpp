/*
 *  main_qt_gui.cpp
 *  climso-auto
 *
 *  Created by Maël Valais on 10/06/2014.
 *  Copyright (c) 2014 Maël Valais. All rights reserved.
 *
 *  Permet de lancer l'environnement de la fenetre principale.
 *  La fonction exec() lance la boucle d'events de la QApplication.
 */

#include "fenetreprincipale.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FenetrePrincipale w;
    w.show();
    return a.exec();
}
