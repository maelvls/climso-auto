####################################################################################
# The testapp_makefile of LPT/USB testing application.
# Author: Jan Soldan, 251 65 Ondrejov 236, Czech Republic
#         Matt Longmire, SBIG
####################################################################################
# How to run this script:
# 1) cd to this directory
# 2) make
# Note: Linker uses shared libraries and do not forget to put
# *.so libraries to the /usr/local/lib directory and 
# setenv LD_LIBRARY_PATH to this path. See README.txt for details. 
####################################################################################
APP_NAME   = testapp
APP_NAME2  = sbig2fits
S_APP      = main.cpp csbigcam.cpp csbigimg.cpp
O_APP      = main.o csbigcam.o csbigimg.o
S_APP2     = sbig2fits.cpp csbigcam.cpp csbigimg.cpp
O_APP2     = sbig2fits.o csbigcam.o csbigimg.o
####################################################################################
APP_SRC    = $(S_APP)
APP_OBJ    = $(O_APP)
APP_SRC2   = $(S_APP2)
APP_OBJ2   = $(O_APP2)
INCLUDE    = -I .
LIBRARY    = -L .
APP_CFLAGS = -O2 $(INCLUDE) $(LIBRARY) -Wall 
APP_CC     = g++
####################################################################################
all: $(APP_NAME) $(APP_NAME2)
####################################################################################
# APP_NAME
####################################################################################
$(APP_NAME): $(APP_OBJ)
	  $(APP_CC) $(APP_CFLAGS) -o $(APP_NAME) $(APP_OBJ) -lsbigudrv -lm
	  chmod a+x $(APP_NAME)

####################################################################################
# APP_NAME2
####################################################################################
$(APP_NAME2): $(APP_OBJ2)
	  $(APP_CC) $(APP_CFLAGS) -o $(APP_NAME2) $(APP_OBJ2) -lsbigudrv -lm
	  chmod a+x $(APP_NAME2)

main.o : main.cpp csbigcam.h csbigimg.h
	  $(APP_CC) $(APP_CFLAGS) -o main.o -c main.cpp

csbigcam.o : csbigcam.h csbigcam.h csbigimg.h
	  $(APP_CC) $(APP_CFLAGS) -o csbigcam.o -c csbigcam.cpp

csbigimg.o : csbigimg.cpp csbigimg.h
	  $(APP_CC) $(APP_CFLAGS) -o csbigimg.o -c csbigimg.cpp

sbig2fits.o : sbig2fits.cpp csbigcam.h csbigimg.h
	  $(APP_CC) $(APP_CFLAGS) -o sbig2fits.o -c sbig2fits.cpp

####################################################################################


clean:
	rm $(APP_NAME) $(APP_NAME2) *.o *.sbig

