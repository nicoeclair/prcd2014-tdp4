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

#ifndef _CLASS_H_
#define _CLASS_H_

#include <stdio.h>

#include "type.h"
#include "type_spec.h"



/*-----------------------------*
 | Geometry class of an entity |-----------------------------------*
 | file   : reads characterization of an entity                    |
 | hit    : computes intersection between a ray and an entity      |
 | point  : contructs a new ray from a Hit to a point of an entity |
 | binder : helps the binder to be built                           |
 | uv2xyz : from uv-coord, gives the xyz-point, normal, tangent    |
 *-----------------------------------------------------------------*/
#include "geo.h"

typedef struct {
  char  *Text;
  GEO * (*file)   (PROT_FILE_GEO);
  BOOL  (*hit)    (PROT_HIT_GEO);
  BOOL  (*point)  (PROT_POINT_GEO);
  void  (*binder) (PROT_BINDER_GEO);
} CLASS_GEO;


extern CLASS_GEO   ClassGeo[];


#endif
