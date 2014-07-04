/*

lpardrv.h - Local options for pardrv.h

*/
#ifndef _LPARDRV_
#define _LPARDRV_


#define  STF_CLASS                                   /* new stuff related to the STF CCD cameras */

#define ENV_WIN                 1                       /* Target for Windows environment */
#define ENV_WINVXD              2                       /* SBIG Use Only, Win 9X VXD */
#define ENV_WINSYS              3                       /* SBIG Use Only, Win NT SYS */
#define ENV_ESRVJK              4                       /* SBIG Use Only, Ethernet Remote */
#define ENV_ESRVWIN             5                       /* SBIG Use Only, Ethernet Remote */
#define ENV_MACOSX              6                       /* SBIG Use Only, Mac OSX */
#define ENV_LINUX               7                       /* SBIG Use Only, Linux */
#define ENV_NIOS            8                   /* SBIG Use Only, Embedded NIOS */

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
#else
	#include <sbigudrv.h>
#endif


#endif
