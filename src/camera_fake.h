//
//  camera_fake.h
//  climso-auto
//
//  Created by Maël Valais on 11/09/2016.
//  Copyright © 2016 Maël Valais. All rights reserved.
//

#ifndef camera_fake_h
#define camera_fake_h

#include "image.h"
#include "camera.h"

class CameraFake : public Camera {
private:
	Image* img;
public:
	CameraFake();
	virtual ~CameraFake();
public:
	bool estConnectee();
	bool connecter();
	bool deconnecter();
	Image* capturer();
	string derniereErreur();
};

#endif /* camera_fake_h */
