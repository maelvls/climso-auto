//
//  main_arduino.c
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <stdio.h>
#include "cmd_arduino.h"

int main(int argc, const char *argv[])
{

	int fd_arduino = allumerCommunication(UNIX_DEVICE);
	if(fd_arduino == 0) {
		printf("Impossible d'ouvrir le device\n");
	}
	char choix[20], dir_choix[20];
	int direction=VERS_NORD, duree, err_choix;
	while (choix[0] != 'q') {
		printf("Que voulez-vous faire ?\n");
		printf("e : envoyer une commande\n");
		printf("q : quitter\n");
		scanf("%s",choix);
		switch (choix[0]) {
			case 'e':
				err_choix = 0;
				printf("Envoyons une commande\n");
				printf("D'abord, quelle direction ? (n,s,o,e)\n");
				scanf("%s",dir_choix);
				switch (dir_choix[0]) {
					case 'n': direction = VERS_NORD; break;
					case 's': direction = VERS_SUD; break;
					case 'o': direction = VERS_OUEST; break;
					case 'e': direction = VERS_EST; break;
					default: err_choix=1; break;
				}
				if(err_choix) {
					printf("Erreur de direction\n");
					break;
				}
				printf("Maintenant, choisissez la duree entre 1 et 65500 ms :\n");
				scanf("%d",&duree);
				if(duree < 1 || duree > 65500) {
					printf("Erreur de duree\n");
					break;
				}
				printf("Envoi de la direction '%s' pendant %d ms : \n",dir_choix,duree);
				int retour = envoyerCommande(direction, duree,fd_arduino);
				if(retour != 0) {
					printf("Une erreur de communication s'est produite (numero %d)\n",retour);
				}
				break;
				
			default:
				break;
		}
	}
	eteindreCommunication(fd_arduino);
	return 0;
}