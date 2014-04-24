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

#define START_BYTE	0xFF
#define SPEED_BAUD	9600


/**
    Envoie les informations à travers le canal série
    
    @param direction La direction 
    @param La duree en ms
    @return 0 si tout s'est bien passé, 1 si problème d'ouverture du /dev/tty...
*/
int envoyerCommande(uint8_t direction, double duree, int fd) {
	/*
		On envoie d'abord un octet pour la commande (entre 3 et 13)
		Puis on envoie deux octets pour la duree (entre 1 et 65500)
	 */

	//
	// Initialisation de la communication
	//
	if(serialport_writebyte(fd, START_BYTE) == -1) {
		printf("Erreur lors de l'envoi du byte START\n");
		return 2;
	}
	//
	// Direction
	//
	if(serialport_writebyte(fd, direction) == -1) {
		printf("Erreur lors de l'envoi de la direction\n");
		return 3;
	}
	//
	// Durée : On envoie en big indian les deux parties du uint16_t
	//
	if(serialport_writebytes(fd,(uint8_t*)&duree,sizeof(double)) == -1) {
		printf("Erreur lors de l'envoi de la direction\n");
		return 4;
	}
    return 0;
}

int allumerCommunication(const char* device) {
	return serialport_init(device, SPEED_BAUD);
}
void eteindreCommunication(int fd_device) {
	serialport_close(fd_device);
}