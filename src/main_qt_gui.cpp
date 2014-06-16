//
//  main_qt_gui.cpp
//  climso-auto
//
//  Created by Maël Valais on 10/06/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include "fenetreprincipale.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FenetrePrincipale w;
    w.show();
    return a.exec();
}
