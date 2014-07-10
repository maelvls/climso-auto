/*
 * main_image.cpp
 *
 *  Created on: 9 juil. 2014
 *      Author: mael65
 */

#include "image.h"



int main(int argc, char **argv) {
	Image* obj = Image::depuisTiff("lapl_obj.tif");
	Image* ref = Image::depuisTiff("lapl_ref.tif");
	Image correl(0,0);
	correl.correlation_rapide(*obj,*ref,0.70);
	correl.versTiff("correl.tif");
}
