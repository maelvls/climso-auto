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
#ifdef __cplusplus
extern "C" {
#endif
	
#define ARDUINO_ERR	-1

int arduinoInitialiserCom(const char* device_file_name);
int arduinoEnvoyerCmd(int pin, int duree, int fd_device);
int arduinoRecevoirReponse(int fd, char* rep);
void arduinoEteindreCom(int fd_device);

#ifdef __cplusplus
}
#endif
#endif /* defined(__climso_auto__cmd_arduino__) */
