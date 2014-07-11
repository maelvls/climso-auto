/*
 * arduino.h
 *
 *  Created on: 6 juin 2014
 *      Author: Maël Valais
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_
#include <string>
#include <iostream>
using namespace std;

#define NO_ERR					0
#define ERR_OUVERTURE_FICHIER 	1
#define ERR_ECRITURE_FICHIER 	2
#define ERR_LECTURE_FICHIER 	3

#define SPEED_BAUD				9600

class Arduino {
private:
	int fd; // unix-like file descriptor
	string path;
	int derniereErreur;
public:
    Arduino(string dev_path);
	~Arduino();
	int EnvoyerCmd(int pin, int duree);
	int EnvoyerCmd(char* cmd);
	int EnvoyerCmd(char* cmd, int longCmd);
    int RecevoirReponse(string &chaine);
	int Initialiser();
	int Flush();
	int LireReponse(char* buf, int longMaxBuf, char carDeFin, int timeout);
	string getDerniereErreurMessage();
	static Arduino* initialiserDepuisListeDePossibilites(string liste_paths_possibles);
    int getErreur();
    string getPath() {return path;}
    bool verifierConnexion();
};

#endif /* ARDUINO_H_ */
