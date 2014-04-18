//
//  cmd-arduino.h
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#ifndef __climso_auto__cmd_arduino__
#define __climso_auto__cmd_arduino__

#include <stdio.h>

#define VERS_NORD   1
#define VERS_EST    2
#define VERS_SUD    3
#define VERS_OUEST  4

int envoyerCommande(int direction, int duree);

#endif /* defined(__climso_auto__cmd_arduino__) */
