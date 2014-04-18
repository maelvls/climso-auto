//
//  cmd-arduino.c
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//


#include "cmd-arduino.h"
#include "arduino-serial-lib.h"


/**
    Envoie les informations à travers le canal série
    @param direction La direction 
    @param La duree en ms
*/
int envoyerCommande(int direction, int duree) {
    int fd;
    fd = serialport_init("/dev/tty.usbmodemfa131", 9600);
    
    char commande [30];
    snprintf(commande, 30, "%d %d", direction, duree);
    serialport_write(fd, commande);
    serialport_close(fd);
}