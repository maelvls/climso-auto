//
//  cmd_arduino.c
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


#include "cmd_arduino.h"
#include "arduino-serial-lib.h"



/**
    Envoi d'informations à travers le canal série
    @param direction La direction 
    @param La duree en ms
    @return 0 si tout s'est bien passé, -1 si problème d'écriture sur le canal
*/
int envoyerCommande(int direction, int duree, int fd) {
	/*
		On concatène le caractère de start, le pin de direction et la duree
	 */
	char commande [30];
	sprintf(commande,"%c %d %d",START_BYTE,direction,duree);
    return (serialport_write(fd,commande)  == -1)?-1:0;
}


/**
 * Initialisation de la communication avec arduino à la vitesse SPEED_BAUDS (9600 bauds)
 * en 8N1 (8 bits de données, pas de parité, un seul bit stop)
 * @param device Le nom du device (/dev/ttyUSB ou quelque chose comme ça)
 * @return -1 si erreur
 */
int allumerCommunication(const char* device) {
	return serialport_init(device, SPEED_BAUD);
}
/**
 * Couper la communication et libérer le canal
 * @param fd_device
 */
void eteindreCommunication(int fd_device) {
	serialport_close(fd_device);
}
