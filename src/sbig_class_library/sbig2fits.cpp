
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

#include "csbigcam.h"


int main(int argc, char *argv[])
{
	CSBIGImg img;

    argc--;
    if (argc < 2) {
        printf("\nsbig2fits infile outfile\n\n");
        exit (-1);
    }

    printf("converting %s from SBIG format to FITS (result in %s)\n",
            argv[1],
            argv[2]);

    if (img.OpenImage(argv[1]) != SBFE_NO_ERROR) {
        printf("error opening input file:%s\n", argv[1]);
        exit (-2);
    }


    if (img.SaveImage(argv[2],SBIF_FITS) != SBFE_NO_ERROR) {
        printf("error saving image to FITS file:%s\n", argv[2]);
        exit (-3);
    }

    return 0;
}

