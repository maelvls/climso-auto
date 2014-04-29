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
	char choix[20], dir_choix[20];
	int direction, duree;
	int fd_arduino = arduinoInitialiserCom("/dev/tty.usbmodemfa131");
	if(fd_arduino == -1) {
		printf("Impossible d'ouvrir le device\n");
		choix[0]='q';
	}
	while (choix[0] != 'q') {
		printf("Que voulez-vous faire ?\n");
		printf("e : envoyer une commande\n");
		printf("q : quitter\n");
		scanf("%s",choix);
		switch (choix[0]) {
			case 'e':
			{
				printf("Envoyons une commande\n");
				printf("Sur quel pin ?");
				scanf("%d",&direction);
				if(direction < 0 || direction > 13) {
					printf("Erreur de choix de pin\n");
					break;
				}
				printf("Tappez la duree en ms :\n");
				scanf("%d",&duree);
				if(duree < 1 || duree > 1000000000) {
					printf("Erreur de duree\n");
					break;
				}
				printf("Envoi d'une impulsion sur le pin %d pendant %d ms\n",direction,duree);
				int retour = arduinoEnvoyerCmd(direction, duree,fd_arduino);
				if(retour != 0) {
					printf("Une erreur de communication s'est produite (numero %d)\n",retour);
				}
				break;
			}
			default:
				break;
		}
	}
	arduinoEteindreCom(fd_arduino);
	return 0;
}
