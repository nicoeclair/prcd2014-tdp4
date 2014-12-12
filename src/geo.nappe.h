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

#ifndef _GEO_NAPPE_H_
#define _GEO_NAPPE_H_

#include <stdio.h>

#include "type.h"
#include "type_spec.h"
#include "geo.h"

extern GEO *file_geo_nappe   (BYTE Type, FILE *File);
extern void binder_geo_nappe (GEO *Generic, INDEX *Index, VECTOR *Min,
			      VECTOR *Max, void **Info);
extern BOOL point_geo_nappe  (GEO *Generic, HIT *Hit, RAY *NewRay);
extern BOOL hit_geo_nappe    (GEO *Generic, RAY *Ray, HIT *Hit, PAIR *Bound,
			      void *Info);

#endif
