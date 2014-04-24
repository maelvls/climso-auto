//
//  cmd_arduino.h
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//
//
//  Les commandes de ce fichier permettent d'envoyer à l'arduino
//  les commandes.
//  NOTE : il faut changer UNIX_DEVICE par le bon /dev/tty...
//          ça peut être /dev/tty.usbmodemfa131 pour un Mac,
//          ou alors /dev/ttyAMC0 sous unix...
//

#ifndef __climso_auto__cmd_arduino__
#define __climso_auto__cmd_arduino__

#include <stdio.h>
#include <stdint.h>

#define UNIX_DEVICE "/dev/tty.usbmodemfa131"

#define VERS_NORD   4
#define VERS_EST    5
#define VERS_SUD    6
#define VERS_OUEST  7

int allumerCommunication(const char* device);
int envoyerCommande(uint8_t direction, double duree, int fd_device);
void eteindreCommunication(int fd_device);



#endif /* defined(__climso_auto__cmd_arduino__) */
