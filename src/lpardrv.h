/*

lpardrv.h - Local options for pardrv.h

*/
#ifndef _LPARDRV_
#define _LPARDRV_

#define  STF_CLASS						/* new stuff related to the STF CCD cameras */

#if __APPLE__
 #include <SBIGUDrv/sbigudrv.h>
#else
 #include <sbigudrv.h>
#endif


#endif // _LPARDRV_
