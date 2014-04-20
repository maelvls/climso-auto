//
//  cmd-arduino.c
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

#define UNIX_DEVICE "/dev/tty.usbmodemfa131"


#include "cmd-arduino.h"
#include "arduino-serial-lib.h"


/**
    Envoie les informations à travers le canal série
    
    @param direction La direction 
    @param La duree en ms
    @return 1 si tout s'est bien passé, 0 si problème d'ouverture du /dev/tty...
*/
int envoyerCommande(int direction, int duree) {
    int fd;
    fd = serialport_init(UNIX_DEVICE, 9600);
    if(fd == 0) // Impossible de lire sur ce device
        return 0;
    char commande [30];
    printf(commande, 30, "%d %d", direction, duree);
    serialport_write(fd, commande);
    serialport_close(fd);
    return 1;
}