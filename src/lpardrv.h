/*

lpardrv.h - Local options for pardrv.h

*/
#ifndef _LPARDRV_
#define _LPARDRV_

#define  STF_CLASS						/* new stuff related to the STF CCD cameras */

#if APPLE
 #include <SBIGUDrv/sbigudrv.h>
 #ifdef _DEBUG
  #define _DEBUG		1
 #endif
#else
 #include "sbigudrv.h"
#endif

#if _DEBUG
 #ifdef _DEBUG
  #define _DEBUG 1
#endif
#endif

#endif
