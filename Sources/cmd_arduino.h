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

#define START_BYTE	'a'
#define SPEED_BAUD	9600

#define MAC_DEVICE "/dev/tty.usbmodemfa131"

#define VERS_NORD   4
#define VERS_EST    5
#define VERS_SUD    6
#define VERS_OUEST  7

#define PIN_MIN		0
#define PIN_MAX		13

#define DUREE_MIN	1
#define DUREE_MAX	100000

int allumerCommunication(const char* device);
int envoyerCommande(int direction, int duree, int fd_device);
void eteindreCommunication(int fd_device);



#endif /* defined(__climso_auto__cmd_arduino__) */
