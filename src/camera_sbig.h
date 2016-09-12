//
//  camera.h
//  climso-auto
//
//  Created by Maël Valais on 11/09/2016.
//  Copyright © 2016 Maël Valais. All rights reserved.
//

#ifndef camera_sbig_h
#define camera_sbig_h

#include "csbigcam.h"
#include "csbigimg.h"
#include "image.h"
#include "camera.h"

#define DUREE_EXPOSITION		120 	// en ms (FIXME: j'ai l'impression que cela ne change rien pour < 120ms)

class CameraSBIG : public Camera {
private:
	CSBIGCam* cam;
	Image* img;
	//CSBIGImg* img_sbig;
public:
	CameraSBIG();
	virtual ~CameraSBIG();
public:
	bool estConnectee();
	bool connecter();
	bool deconnecter();
	Image* capturer();
	string derniereErreur();
};

#endif /* camera_sbig_h */
