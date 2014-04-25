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

	int fd_arduino = allumerCommunication(MAC_DEVICE);
	if(fd_arduino == 0) {
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
				printf("Sur quel pin ? (de %d à %d)\n",PIN_MIN,PIN_MAX);
				scanf("%d",&direction);
				if(direction < PIN_MIN || direction > PIN_MAX) {
					printf("Erreur de choix de pin\n");
					break;
				}
				printf("Tappez la duree entre %d et %d ms :\n",DUREE_MIN,DUREE_MAX);
				scanf("%d",&duree);
				if(duree < DUREE_MIN || duree > DUREE_MAX) {
					printf("Erreur de duree\n");
					break;
				}
				printf("Envoi de la direction '%s' pendant %d ms : \n",dir_choix,duree);
				int retour = envoyerCommande(direction, duree,fd_arduino);
				if(retour != 0) {
					printf("Une erreur de communication s'est produite (numero %d)\n",retour);
				}
				break;
			}
			default:
				break;
		}
	}
	eteindreCommunication(fd_arduino);
	return 0;
}
