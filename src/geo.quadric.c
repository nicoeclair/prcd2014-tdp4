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

#include "lanceur.h"

#include "geo.h"
#include "geo.quadric.h"


typedef struct {
  BYTE    Type;
  VECTOR  Min, Max;
  VECTOR  Point;
  REAL    Radius;
} GEO_SPHERE;

GEO *
file_geo_sphere (BYTE Type, FILE *File)
{
  GEO_SPHERE  *Geo;
  VECTOR      Vector;

  INIT_MEM (Geo, 1, GEO_SPHERE);
  Geo->Type = Type;
  GET_VECTOR (Geo->Point);
  GET_REAL (Geo->Radius);
  Vector.x = Vector.y = Vector.z = Geo->Radius;
  VEC_SUB (Geo->Min, Geo->Point, Vector);
  VEC_ADD (Geo->Max, Geo->Point, Vector);

  return ((GEO *) Geo);
}

void
binder_geo_sphere (GEO *Generic, INDEX *Index, VECTOR *Min, VECTOR *Max, void **Info)
{
  GEO_SPHERE *Geo;

  Geo = (GEO_SPHERE *) Generic;
  *Min = Geo->Min;
  *Max = Geo->Max;
  *Info = NULL;
}

/***********************************
 * xyz-coordinate to uv-coordinate *
 ***********************************/
static BOOL xyz2uv_geo_sphere (GEO_SPHERE *Geo, HIT * Hit)
{
    REAL   u, v;
    REAL   x, y, z;

    x = (Hit->Point.x - Geo->Point.x);
    y = (Hit->Point.y - Geo->Point.y);
    z = (Hit->Point.z - Geo->Point.z);

    v = (z/Geo->Radius + 1.0)/2.0;
    u = atan2(y,x) / (2.0*PI);
    if (u < 0.0)
	u += 1.0;

    if (u < 0.0)
        u = 0.0;
    if (u > 1.0)
        u = 1.0;
    if (v < 0.0)
        v = 0.0;
    if (v > 1.0)
        v = 1.0;

    Hit->u = u;
    Hit->v = v;

    return (TRUE);
}

/*************************************************
 * Transform uv-coordinates into xyz-coordinates *
 *************************************************/
static void uv2xyz_geo_sphere (GEO *Generic, REAL U, REAL V, VECTOR *Point,
			       VECTOR * Normal, VECTOR *Tangent)
{
    GEO_SPHERE   *Geo;
    REAL         CosPnt, SinPnt;
    REAL         r, Real;

    Geo = (GEO_SPHERE *) Generic;

    CosPnt = cos(2.0*PI*U);
    SinPnt = sin(2.0*PI*U);
    r = 2.0 * Geo->Radius * sqrt(V*(1.0-V));
    Point->x = Geo->Point.x + r * CosPnt;
    Point->y = Geo->Point.y + r * SinPnt;
    Point->z = Geo->Point.z + Geo->Radius * (2.0*V-1);

    if (Normal != NULL)
    {
        VEC_SUB (*Normal, *Point, Geo->Point);
	VEC_UNIT (*Normal, Real);
    }

    if (Tangent != NULL)
    {
        VEC_MAKE (*Tangent, -SinPnt, CosPnt, 0.0);
    }
}

BOOL point_geo_sphere (GEO *Generic, HIT *Hit, RAY *NewRay)
{
  VECTOR     Point;

  if (NewRay == NULL)
  {
    uv2xyz_geo_sphere (Generic, Hit->u, Hit->v, &(Hit->Point),
		       &(Hit->Normal), NULL);
  }
  else
  {
    uv2xyz_geo_sphere (Generic, Hit->u, Hit->v, &Point, NULL, NULL);
    NewRay->Point = Hit->Point;
    VEC_SUB (NewRay->Vector, Point, NewRay->Point);
    VEC_UNIT (NewRay->Vector, NewRay->Distance);
  }

  return (TRUE);
}

BOOL hit_geo_sphere (GEO *Generic, RAY *Ray, HIT *Hit, PAIR *Bound, void *Info)
{
  GEO_SPHERE  *Geo;
  VECTOR      Point;
  REAL        a, b, Distance, Delta;

  Geo = (GEO_SPHERE *) Generic;

  VEC_SUB (Point, Ray->Point, Geo->Point);
  a = - VEC_DOT (Point, Ray->Vector);
  b = VEC_DOT (Point, Point);
  Delta = a * a - b + Geo->Radius * Geo->Radius;
  if (Delta < 0.0)
    return (FALSE);

  Delta = sqrt (Delta);
  Distance = a - Delta;   /* - Epsilon;*/
  if (Distance < Bound->u || Distance > Bound->v)
    return (FALSE);

  if (Hit) {
    Ray->Distance = Distance;
    VEC_LIN (Point, Ray->Point, Distance, Ray->Vector);
    VEC_SUB (Hit->Normal, Point, Geo->Point);
    if (VEC_DOT (Ray->Vector, Hit->Normal) > 0.0)
      VEC_NEG (Hit->Normal);
    VEC_UNIT (Hit->Normal, Distance);
    VEC_LIN (Hit->Point, Point, Epsilon, Hit->Normal);
    xyz2uv_geo_sphere (Geo, Hit);
  }

  return (TRUE);
}


