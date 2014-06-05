//
//  cmd_arduino.c
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  2014, Maël Valais
//
//  D'après arduino-serial-lib -- simple library for reading/writing serial ports
// 	2006-2013, Tod E. Kurt, http://todbot.com/blog/
//
//  Primitives permettant d'envoyer à l'arduino les commandes.
//  NOTE : il faut changer UNIX_DEVICE par le bon /dev/tty...
//          ça peut être /dev/tty.usbmodemfa131 pour un Mac,
//          ou alors /dev/ttyAMC0 sous unix...
//


#include "cmd_arduino.h"
#include <stdio.h>    	// (Lib C) fonctions C standards pour les input/output (fprintf,...)
#include <string.h>		// (Lib C) Fonctions sur les chaines (strlen...)
#include <errno.h>    	// Définition des numéros d'erreur (perror...)

#define _GNU_SOURCE // Ajouté car pose des pbms pour termios.h car certaines macros
// n'appartiennent pas à la norme POSIX
// Cf. http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html

#include <termios.h>  	// (POSIX) fonctions UNIX de contrôle du terminal (tcsetattr...)
#include <fcntl.h>		// (POSIX) fonctions UNIX d'ouverture de fichiers (open...)
#include <unistd.h>		// (POSIX) fonctions UNIX d'opération sur les fichiers (write...)


#define SPEED_BAUD	B9600


/**
    Envoi d'informations à travers le canal série
    @param pin Le pin sur lequel envoyer l'impulsion
    @param duree La duree en ms
    @param fd Le descripteur de fichier type UNIX ciblant l'arduino
    @return ARDUINO_OK si tout s'est bien passé, ARDUINO_ERR si problème d'écriture sur le canal

    Principe : On concatène le numéro du pin et la duree et on envoie
*/
int arduinoEnvoyerCmd(int pin, int duree, int fd) {
	char commande [30];
	sprintf(commande,"%d,%d",pin,duree);
	printf("Envoi de la commande '%s' (%d ms au pin %d)\n",commande,duree,pin);
	size_t writen = write(fd,commande,strlen(commande)+1);
	return (writen == strlen(commande)+1)?0:ARDUINO_ERR;
}

/**
 * Lit les messages envoyés par arduino (maximum 300 caractères)
 * @param fd Le descripteur de fichier type UNIX ciblant l'arduino
 * @param rep
 * @return
 */
int arduinoRecevoirReponse(int fd, char* rep) {
	const int maxLongRep = 300;
	rep[0]='\0';
	int longueurLue = 0;
	int longueur = lseek(fd,0,SEEK_END);
	if(longueur >= maxLongRep) {
		longueurLue = maxLongRep;
		rep[maxLongRep-1]='\0';
	}
	size_t answerRead = read(fd,rep,longueurLue);
	return (answerRead);
}


/**
 * Initialisation de la communication avec arduino à la vitesse SPEED_BAUDS (9600 bauds)
 * en 8N1 (8 bits de données, pas de parité, un seul bit stop)
 * @param device_file_name_list Une liste contenant les noms des fichiers possibles du device
 * 		(/dev/ttyACM0 par exemple) séparés par des espaces.
 * @return ARDUINO_ERR si erreur, sinon le descripteur de fichier type UNIX ciblant l'arduino
 *
 * @author 2006-2013, Tod E. Kurt, http://todbot.com/blog/ (arduino-serial-lib)
 * @author 2014 Mael Valais pour des modifications
 */
int arduinoInitialiserCom(const char* device_file_name_list) {
	char list_cpy[300];
	strcpy(list_cpy,device_file_name_list);
	struct termios toptions;
	char *device_file_name=NULL;
	int fd = ARDUINO_ERR;
	device_file_name = strtok(list_cpy, " ,;");
	while(device_file_name && fd == -1) {
		fprintf(stderr,"arduinoInitialiserCom: Essai d'ouverture de %s\n",device_file_name);
		fd=open(device_file_name, O_RDWR|O_NONBLOCK);
		if(fd == -1)
			perror("arduinoInitialiserCom");
		device_file_name = strtok(NULL, " ,;");
	}

	if(fd == ARDUINO_ERR) {
		fprintf(stderr,"arduinoInitialiserCom: Impossible d'ouvrir l'un des fichiers de la liste %s\n",device_file_name_list);
		return ARDUINO_ERR;
	}

	if (tcgetattr(fd, &toptions) < 0) { // On récupère la config terminal du fichier
		fprintf(stderr,"arduinoInitialiserCom: Impossible de récupérer les attributs termios pour %s\n",device_file_name);
		perror("arduinoInitialiserCom"); return ARDUINO_ERR;
	}
	cfsetispeed(&toptions, SPEED_BAUD); // On met le transfert à 9600 bauds
	cfsetospeed(&toptions, SPEED_BAUD);

	// Mise en place du protocole série en 8N1 (8 bits par caractère, pas de parité, un bit stop)
	toptions.c_cflag &= ~PARENB; // On désactive le bit de parité (~ pour complément à un)
	toptions.c_cflag &= ~CSTOPB; // On désactive le second bit de stop
	toptions.c_cflag &= ~CSIZE; // On désactive le réglage de taille de caractère
	toptions.c_cflag |= CS8; // On réactive la taille de caractère à 8 bits
	//toptions.c_cflag &= ~CRTSCTS; // On désactive le contrôle de flux FIXME: desactive car pbm termios.h

	toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

	toptions.c_cc[VMIN]  = 0; // see http://unixwiz.net/techtips/termios-vmin-vtime.html
	toptions.c_cc[VTIME] = 0;
	//toptions.c_cc[VTIME] = 20;

	tcsetattr(fd, TCSANOW, &toptions); // On applique la configuration terminal sur le fichier
	if(tcsetattr(fd, TCSAFLUSH, &toptions) < 0) {
		fprintf(stderr,"arduinoInitialiserCom: Impossible de configurer les paramètre terminal du fichier %s\n",device_file_name);
		perror("arduinoInitialiserCom"); return ARDUINO_ERR;
	}
	return fd;
}

/**
 * Couper la communication et libérer le canal
 * @param fd_device Le descripteur de fichier type UNIX ciblant l'arduino
 *
 */
void arduinoEteindreCom(int fd_device) {
	close(fd_device);
}
