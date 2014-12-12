/***************************************************************************
 *                       Lanceur de rayons simplifie                       *
 ***************************************************************************
 * ENSERB, 3me annee Informatique                               1995-1996  *
 * Option AAMP, Module MP2                                                 *
 *                                                                         *
 ***************************************************************************
 * Author : Gilles Subrenat                                                *
 * Date   : 15 janvier 1996                                                *
 *                                                                         *
 * $Id$
 ***************************************************************************/

#ifndef _ETT_H_
#define _ETT_H_

#include <stdio.h>

#include "type.h"
#include "geo.h"

/*--------------------*
 | Optic of an entity |
 *--------------------*/
#define OPT_LIGHT   "LIGHT"
#define OPT_NOLIGHT "NOLIGHT"

typedef struct {
        BOOL    Light;
        COLOR   Color;
        REAL    MinRough,
                MaxRough;
} OPT;


typedef struct {
  OPT   Opt;
  GEO  *Geo;
} ETT;

extern ETT *file_ett (FILE *File);

extern INDEX  NbrEtt;
extern ETT    **MainEtt;
extern INDEX  NbrLgh;
extern ETT    **MainLgh;

#endif
