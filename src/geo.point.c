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


#include <stdio.h>

#include "const.h"
#include "macro.h"
#include "type.h"
#include "type_spec.h"

#include "geo.h"
#include "geo.point.h"


typedef struct {
        BYTE    Type;
	VECTOR	Min, Max;
	VECTOR	Point;
} GEO_POINT;


GEO * file_geo_point (BYTE Type, FILE *File)
{
  GEO_POINT	*Geo;

  INIT_MEM (Geo, 1, GEO_POINT);
  Geo->Type = Type;
  GET_VECTOR (Geo->Point);
  Geo->Min = Geo->Max = Geo->Point;
  return ((GEO *) Geo);
}

void binder_geo_point (GEO *Generic, INDEX *Index, VECTOR *Min, VECTOR *Max, void **Info)
{
  GEO_POINT  *Geo;

  Geo = (GEO_POINT *) Generic;
  *Min = Geo->Min;
  *Max = Geo->Max;
  *Info = NULL;
}

static void uv2xyz_geo_point (GEO *Generic, REAL U, REAL V, VECTOR *Point,
			      VECTOR * Normal, VECTOR *Tangent)
{
    GEO_POINT   *Geo;
    REAL         CosPnt, SinPnt;
    REAL         r;

    Geo = (GEO_POINT *) Generic;

    *Point = Geo->Point;

    if ((Normal != NULL) || (Tangent != NULL))
    {
        CosPnt = cos(2.0*PI*U);
        SinPnt = sin(2.0*PI*U);
        r = 2.0 * sqrt(V*(1.0-V));
    }

    if (Normal != NULL)
    {
        Normal->x = r * CosPnt;
        Normal->y = r * SinPnt;
        Normal->z = 2.0*V-1.0;
/*	VEC_UNIT (*Normal, Real);  unuseful */
    }

    if (Tangent != NULL)
    {
        VEC_MAKE (*Tangent, -SinPnt, CosPnt, 0.0);
    }
}

/*************************************************
 * Give a point of a nappe                       *
 * if NewRay == NULL then fill Hit-structure     *
 *                   else fille NewRay-structure *
 *************************************************/
BOOL point_geo_point (GEO *Generic, HIT *Hit, RAY *NewRay)
{
  VECTOR    Point;

  if (NewRay == NULL)
  {
    uv2xyz_geo_point (Generic, Hit->u, Hit->v, &(Hit->Point),
		      &(Hit->Normal), NULL);
  }
  else {
    uv2xyz_geo_point (Generic, Hit->u, Hit->v, &Point, NULL, NULL);
    NewRay->Point = Hit->Point; 
    VEC_SUB (NewRay->Vector, Point, NewRay->Point);
/*    if (VEC_DOT (NewRay->Vector, Hit->Normal) < 0.0) return (FALSE);*/
    VEC_UNIT (NewRay->Vector, NewRay->Distance);
  }

  return (TRUE);
}

BOOL hit_geo_point (GEO *Generic, RAY *Ray, HIT *Hit, PAIR *Bound, void *Info)
{
  return (FALSE);
}
