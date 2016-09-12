//
//  camera.h
//  climso-auto
//
//  Created by Maël Valais on 11/09/2016.
//  Copyright © 2016 Maël Valais. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include "image.h"

class Camera {
public:
	virtual bool estConnectee() =0;
	virtual bool connecter() =0;
	virtual bool deconnecter() =0;
	virtual Image* capturer() =0;
	virtual string derniereErreur() =0;
};

#endif /* camera_h */
