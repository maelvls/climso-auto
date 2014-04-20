//
//  main_arduino.c
//  climso-auto
//
//  Created by Maël Valais on 18/04/2014.
//  Copyright (c) 2014 Maël Valais. All rights reserved.
//

#include <stdio.h>
#include "cmd-arduino.h"

int main(int argc, const char *argv[])
{
	envoyerCommande(VERS_NORD, 10000);
	return 0;
}