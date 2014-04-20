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

#define VERS_NORD   4
#define VERS_EST    5
#define VERS_SUD    6
#define VERS_OUEST  7

int envoyerCommande(int direction, int duree);

#endif /* defined(__climso_auto__cmd_arduino__) */
