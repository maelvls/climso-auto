/*
 * arduino.cpp
 *
 *  Created on: 6 juin 2014
 *      Author: admin
 */

#include <cstdio>
#include <string.h>		// (Lib C) Fonctions sur les chaines (strlen...)
#include <errno.h>    	// Définition des numéros d'erreur (perror...)

#define _GNU_SOURCE // Ajouté car pose des pbms pour termios.h car certaines macros
// n'appartiennent pas à la norme POSIX
// Cf. http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html

#include <termios.h>  	// (POSIX) fonctions UNIX de contrôle du terminal (tcsetattr...)
#include <fcntl.h>		// (POSIX) fonctions UNIX d'ouverture de fichiers (open...)
#include <unistd.h>		// (POSIX) fonctions UNIX d'opération sur les fichiers (write...)


#include "arduino.h"


/**
 * Initialisation de la communication avec arduino à la vitesse SPEED_BAUDS (9600 bauds)
 * en 8N1 (8 bits de données, pas de parité, un seul bit stop)
 * @param dev_path Path du device (/dev/ttyACM0 par exemple)
 * @return NO_ERR si tout s'est bien passé, ERR_qqchose si problème d'écriture sur le canal
 *
 * @author 2006-2013, Tod E. Kurt, http://todbot.com/blog/ (arduino-serial-lib)
 * @author 2014 Mael Valais pour des modifications
 */
Arduino::Arduino(string dev_path) {
	derniereErreur = NO_ERR;
    path = dev_path;

    fd = open(path.c_str(), O_RDWR|O_NONBLOCK);
    if(fd == -1) {
        derniereErreur = ERR_OUVERTURE_FICHIER;
		return;
	}

	struct termios toptions;
    if (tcgetattr(fd, &toptions) < 0) { // On récupère la config terminal du fichier
		derniereErreur = ERR_LECTURE_FICHIER; return;
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
		derniereErreur = ERR_ECRITURE_FICHIER; return;
	}
}

/**
 * Couper la communication et libérer le canal
 */
Arduino::~Arduino() {
    if(fd != -1)
        close(fd);
}

/**
    Envoi d'informations à travers le canal série
    @param pin Le pin sur lequel envoyer l'impulsion
    @param duree La duree en ms
    @return NO_ERR si tout s'est bien passé, ERR_qqchose si problème d'écriture sur le canal

    Principe : On concatène le numéro du pin et la duree et on envoie
*/
int Arduino::EnvoyerCmd(int pin, int duree) {
	int derniereErreur = NO_ERR;
	char commande [30];
	sprintf(commande,"%d,%d",pin,duree);
	int donneesEcrites = write(fd,commande,strlen(commande)+1);
		strlen(commande)+1;
	if(donneesEcrites < strlen(commande)+1) {
		derniereErreur = ERR_ECRITURE_FICHIER;
	}
	else derniereErreur = NO_ERR;
	return derniereErreur;
}

/**
 * Lit les messages envoyés par arduino (maximum 300 caractères)
 * @param fd Le descripteur de fichier type UNIX ciblant l'arduino
 * @param rep
 * @return NO_ERR si tout s'est bien passé, ERR_qqchose si problème d'écriture sur le canal
 */
int Arduino::RecevoirReponse(string &chaine) {
	int derniereErreur = NO_ERR;
	int maxDonneesLues = 300;
	char donneesLues [maxDonneesLues];
	donneesLues[0]='\0';
	int longueurLue = 0;
	int longueur = lseek(fd,0,SEEK_END);
	if(longueur >= maxDonneesLues) {
		longueurLue = maxDonneesLues;
		donneesLues[maxDonneesLues-1]='\0';
	}
	size_t longueurEffective = read(fd,donneesLues,longueurLue);
    if(longueurEffective > 0) {
		chaine = string(donneesLues);
		derniereErreur = NO_ERR;
    } else {
        chaine = "";
		derniereErreur = ERR_LECTURE_FICHIER;
	}
	return derniereErreur;
}

string Arduino::getDerniereErreurMessage() {
    switch (derniereErreur) {
    case NO_ERR:
        return "Pas d'erreur";
    case ERR_OUVERTURE_FICHIER:
        return "Impossible d'ouvrir le fichier "+path+", droits insuffisants ou fichier introuvable";
    case ERR_LECTURE_FICHIER:
        return "Impossible de lire dans le fichier "+path;
    case ERR_ECRITURE_FICHIER:
        return "Impossible d'ecrire dans le fichier "+path;


    default:
        return "Erreur inconnue";
	}
}
/**
 * Essaie de créer l'objet Arduino à partir de la liste (séparée par des espaces)
 * des différents paths possibles ("/dev/ttyACM0 /dev/usb0" par exemple)
 * NOTE: on ne pourra pas savoir quelles erreurs sont apparues sur les
 * devices en début de liste ; on ne pourra savoir l'erreur que sur le dernier
 * de la liste.
 * @param liste_paths_possibles
 * @return L'objet créé ; attention, une erreur est peut etre survenue
 */
Arduino* Arduino::initialiserDepuisListeDePossibilites(
		string liste_paths_possibles) {
	Arduino* ard;
	int err = NO_ERR;
	char liste[300], *token;
	strcpy(liste,liste_paths_possibles.c_str());
	token = strtok(liste, " ,;");
	while(token && err != NO_ERR) {
		if(ard) delete ard;
		ard = new Arduino(token);
		token = strtok(NULL, " ,;");
	}
	return ard;
}

int Arduino::getErreur() {
    return derniereErreur;
}