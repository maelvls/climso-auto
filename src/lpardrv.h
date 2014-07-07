/*
 * lpardrv.h - Local options for pardrv.h
 *
 * Si la machine semble ne pas reconnaitre les macros système __LINUX__, __APPLE__
 * et autres, vérifiez que la macro système est bien définie pour cette machine :
 * 		touch essai.h; cpp -dM essai.h; rm essai.h
 * Cela donnera une liste des macros définies lors de la compilation. J'ai
 * tenté d'indiquer suffisement de macros possibles pour qu'aucun problème n'apparaisse,
 * mais on est jamais trop sûr...
 *
 */


#ifndef _LPARDRV_
#define _LPARDRV_

#define STF_CLASS					/* new stuff related to the STF-8300, STF-8050, etc. cameras */

#define ENV_WIN				1			/* Target for Windows environment */
#define ENV_WINVXD		2			/* SBIG Use Only, Win 9X VXD */
#define ENV_WINSYS		3			/* SBIG Use Only, Win NT SYS */
#define ENV_ESRVJK		4			/* SBIG Use Only, Ethernet Remote */
#define ENV_ESRVWIN		5			/* SBIG Use Only, Ethernet Remote */
#define ENV_MACOSX		6			/* SBIG Use Only, Mac OSX */
#define ENV_LINUX			7			/* SBIG Use Only, Linux */
#define ENV_NIOS	    8			/* SBIG Use Only, Embedded NIOS */

#if defined(__APPLE__) or defined(APPLE) or defined(apple)
#define TARGET ENV_MACOSX
#endif

#if defined(LINUX) or defined(__LINUX__) or defined(linux)
#define TARGET ENV_LINUX
#endif

#ifndef TARGET
#define TARGET ENV_LINUX       /* Set for your default target */
#endif

#if TARGET == ENV_MACOSX
	#include <SBIGUDrv/sbigudrv.h>
	#ifdef _DEBUG
			#define _DEBUG		1
	#endif

#else
	#include "sbigudrv.h"
#endif

#if TARGET == ENV_LINUX
	#include <libusb.h>
	#ifdef _DEBUG
			#define _DEBUG 1
	#endif
#endif

#endif
