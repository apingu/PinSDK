//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef _WRITEPINEXTRADATA_H
#define _WRITEPINEXTRADATA_H

#include "save_pin.h"
#include <pf-stdlib.h>


int  WriteExtraData( uPfFile fh,
                     unsigned short AdtumMarkX, 
                     unsigned short AdtumMarkY, 
                     unsigned char  Act,
                     unsigned char  Direction,
                     unsigned char  Step );


int  WriteExtraZLine( uPfFile fh, 
                      unsigned short zx0,
                      unsigned short zy0, 
                      unsigned short zx1,
                      unsigned short zy1 );


int  WriteExtraOffset( uPfFile fh,
                        unsigned short x,
                       unsigned short y );


int  WriteExtraHitPoint ( uPfFile fh,
                           unsigned short x,
                          unsigned short y );


int  WriteExtraRect     ( uPfFile fh,
                          unsigned short shape,
                          unsigned short tall,
                          unsigned short top,
                          unsigned short left,
                          unsigned short right,
                          unsigned short bottom );
#endif

