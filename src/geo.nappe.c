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
#include "geo.nappe.h"


/********************
 * Type definitions *
 ********************/

typedef	struct {
	VECTOR	Point;
	VECTOR	Normal;
} PNT;

typedef struct {
	INDEX	i, j, k, l;
	VECTOR	Normal;
	INDEX   NumFct;
} FCT;

typedef struct {
	BYTE	Type;
	VECTOR	Min, Max;
	INDEX	NbrPnt, NbrFct;
	PNT	*TabPnt;
	FCT	*TabFct;
} GEO_NAPPE;


/*****************************************
 * Read nappe characterization in a file *
 *****************************************/
GEO *
file_geo_nappe (BYTE Type, FILE *File)
{
  GEO_NAPPE  *Geo;
  PNT        *Pnt, *PntA, *PntB, *PntC, *PntD;
  FCT        *Fct;
  VECTOR     U, V;
  REAL       Real;
  INDEX      Index;

  INIT_MEM (Geo, 1, GEO_NAPPE);
  Geo->Type = Type;
  GET_INDEX (Geo->NbrPnt);
  INIT_MEM (Geo->TabPnt, Geo->NbrPnt, PNT);
  GET_INDEX (Geo->NbrFct);
  INIT_MEM (Geo->TabFct, Geo->NbrFct, FCT);
  Geo->Min.x = Geo->Min.y = Geo->Min.z =  INFINITY;
  Geo->Max.x = Geo->Max.y = Geo->Max.z = -INFINITY;

  for (Index = 0, Pnt = Geo->TabPnt; Index < Geo->NbrPnt; Index++, Pnt++) {
    GET_VECTOR (Pnt->Point);
    VEC_MIN (Geo->Min, Pnt->Point);
    VEC_MAX (Geo->Max, Pnt->Point);
  }

  for (Index = 0, Fct = Geo->TabFct; Index < Geo->NbrFct; Index++, Fct++) {
    if (fscanf (File, " ( %d %d %d %d )", &Fct->i, &Fct->j, &Fct->k, &Fct->l) < 4)
      return (FALSE);
    Fct->NumFct = Index;
    PntA = Geo->TabPnt + Fct->i; 
    PntB = Geo->TabPnt + Fct->j;
    PntC = Geo->TabPnt + Fct->k;
    PntD = Geo->TabPnt + Fct->l;
    VEC_SUB (U, PntC->Point, PntA->Point);
    VEC_SUB (V, PntD->Point, PntB->Point);
    VEC_CROSS (Fct->Normal, U, V);
    VEC_UNIT (Fct->Normal, Real);
    VEC_INC (PntA->Normal, Fct->Normal);
    VEC_INC (PntB->Normal, Fct->Normal);
    VEC_INC (PntC->Normal, Fct->Normal);
    VEC_INC (PntD->Normal, Fct->Normal);
  }

  for (Index = 0, Pnt = Geo->TabPnt; Index < Geo->NbrPnt; Index++, Pnt++)
    VEC_UNIT (Pnt->Normal, Real);

  return ((GEO *) Geo);
}


/*******************************
 * For the binder construction *
 *******************************/
void binder_geo_nappe (GEO *Generic, INDEX *Index, VECTOR *Min, VECTOR *Max, void **Info)
{
  GEO_NAPPE   *Geo;
  FCT         *Fct;
  PNT         *Pnt;

  Geo = (GEO_NAPPE *) Generic;

  if (*Index == 0)
    *Index = Geo->NbrFct;
  *Index -= 1;
  Fct = Geo->TabFct + *Index;
  *Info = Fct;
  Min->x = Min->y = Min->z = INFINITY;
  Max->x = Max->y = Max->z = -INFINITY;
  Pnt = Geo->TabPnt + Fct->i;
  VEC_MIN (*Min, Pnt->Point);
  VEC_MAX (*Max, Pnt->Point);
  Pnt = Geo->TabPnt + Fct->j;
  VEC_MIN (*Min, Pnt->Point);
  VEC_MAX (*Max, Pnt->Point);
  Pnt = Geo->TabPnt + Fct->k;
  VEC_MIN (*Min, Pnt->Point);
  VEC_MAX (*Max, Pnt->Point);
  Pnt = Geo->TabPnt + Fct->l;
  VEC_MIN (*Min, Pnt->Point);
  VEC_MAX (*Max, Pnt->Point);
}



/***********************************
 * xyz-coordinate to uv-coordinate *
 ***********************************/
static BOOL xyz2uv_geo_nappe (GEO_NAPPE *Geo, HIT * Hit, FCT * Fct)
{
    return (TRUE);
}


/*************************************************
 * Transform uv-coordinates into xyz-coordinates *
 *************************************************/
static void uv2xyz_geo_nappe (GEO *Generic, REAL U, REAL V, VECTOR *Point,
			      VECTOR * Normal, VECTOR *Tangent)
{
    GEO_NAPPE   *Geo;
    INDEX       Num;
    PNT         *A, *B, *C, *D;
    REAL        Real;

    Geo = (GEO_NAPPE *) Generic;

    
    Num = (INDEX) (U * Geo->NbrFct);
    if (Num == Geo->NbrFct)
        Num -= 1;

    A = &(Geo->TabPnt[(Geo->TabFct[Num]).i]);
    B = &(Geo->TabPnt[(Geo->TabFct[Num]).j]);
    C = &(Geo->TabPnt[(Geo->TabFct[Num]).k]);
    D = &(Geo->TabPnt[(Geo->TabFct[Num]).l]);

    VEC_UV (*Point, U, V, A->Point, B->Point, C->Point, D->Point);

    if (Normal != NULL)
    {
        VEC_UV (*Normal, U, V, A->Normal, B->Normal, C->Normal, D->Normal);
        /* or *Normal = (Geo->TabFct[Num]).Normal without any interpolation */
        VEC_UNIT (*Normal, Real);
    }

    if (Tangent != NULL)
    {
        VEC_SUB (*Tangent, (*C).Point, (*A).Point);
        VEC_UNIT (*Tangent, Real);
    }
}


/*************************************************
 * Give a point of a nappe                       *
 * if NewRay == NULL then fill Hit-structure     *
 *                   else fill NewRay-structure  *
 *************************************************/
BOOL point_geo_nappe (GEO *Generic, HIT *Hit, RAY *NewRay)
{
  VECTOR          Point;

  if (NewRay == NULL)
    {
      uv2xyz_geo_nappe (Generic, Hit->u, Hit->v, &(Hit->Point),
			&(Hit->Normal), NULL);
    }
  else
    {
      uv2xyz_geo_nappe (Generic, Hit->u, Hit->v, &Point, NULL, NULL);
      NewRay->Point = Hit->Point;
      VEC_SUB (NewRay->Vector, Point, NewRay->Point);
      VEC_UNIT (NewRay->Vector, NewRay->Distance);
    }

  return (TRUE);
}


/********************************************************
 * Search for an intersection between a nappe and a ray *
 *******************************************************/
BOOL hit_geo_nappe (GEO *Generic, RAY *Ray, HIT *Hit, PAIR *Bound, void *Info)
{
	GEO_NAPPE	*Geo;
	FCT		*Fct;
	PNT	        *Pnt, *PntA, *PntB;
	VECTOR		Normal;
	VECTOR		Point;
	REAL		a, b, c, u, v, uA, vA, uB, vB, Distance, Real;

  Geo = (GEO_NAPPE *) Generic;

  Fct = (FCT *) Info;
  Distance = VEC_DOT (Ray->Vector, Fct->Normal);
  if (ABS(Distance) < EPSILON)
    return (FALSE);
  Pnt = Geo->TabPnt + Fct->i;
  VEC_SUB (Point, Pnt->Point, Ray->Point);
  Distance = VEC_DOT (Point, Fct->Normal) / Distance ; /*-Epsilon est vire*/
  if (Distance < Bound->u || Distance > Bound->v)
    return (FALSE);
  VEC_LIN (Point, Ray->Point, Distance, Ray->Vector);

  if ((ABS(Fct->Normal.z) > ABS(Fct->Normal.x)) && (ABS(Fct->Normal.z) > ABS(Fct->Normal.y))) {
    u = Point.x - Pnt->Point.x; v = Point.y - Pnt->Point.y;
    PntA = Geo->TabPnt + Fct->j; PntB = Geo->TabPnt + Fct->k;
    uA = PntA->Point.x - Pnt->Point.x; vA = PntA->Point.y - Pnt->Point.y;
    uB = PntB->Point.x - Pnt->Point.x; vB = PntB->Point.y - Pnt->Point.y;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA-u*vA) / a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }

    if (Fct->l == Fct->i) 
      return (FALSE);
    
    PntA = Geo->TabPnt + Fct->l;
    uA = PntA->Point.x - Pnt->Point.x; vA = PntA->Point.y - Pnt->Point.y;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA - u*vA)/a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }
    return (FALSE);
  }

  else if (ABS(Fct->Normal.y) > ABS(Fct->Normal.x)) {
    u = Point.z - Pnt->Point.z; v = Point.x - Pnt->Point.x;
    PntA = Geo->TabPnt + Fct->j; PntB = Geo->TabPnt + Fct->k;
    uA = PntA->Point.z - Pnt->Point.z; vA = PntA->Point.x - Pnt->Point.x;
    uB = PntB->Point.z - Pnt->Point.z; vB = PntB->Point.x - Pnt->Point.x;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA - u*vA)/a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }
    
    if (Fct->l == Fct->i) return (FALSE);

    PntA = Geo->TabPnt + Fct->l;
    uA = PntA->Point.z - Pnt->Point.z; vA = PntA->Point.x - Pnt->Point.x;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA - u*vA)/a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }
    return (FALSE);
  }

  else if (ABS(Fct->Normal.x) > EPSILON) {
    u = Point.y - Pnt->Point.y; v = Point.z - Pnt->Point.z;
    PntA = Geo->TabPnt + Fct->j; PntB = Geo->TabPnt + Fct->k;
    uA = PntA->Point.y - Pnt->Point.y; vA = PntA->Point.z - Pnt->Point.z;
    uB = PntB->Point.y - Pnt->Point.y; vB = PntB->Point.z - Pnt->Point.z;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA - u*vA)/a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }
    
    if (Fct->l == Fct->i) return (FALSE);

    PntA = Geo->TabPnt + Fct->l;
    uA = PntA->Point.y - Pnt->Point.y; vA = PntA->Point.z - Pnt->Point.z;
    a = uA*vB - vA*uB; b = (u*vB - v*uB)/a; c = (v*uA - u*vA)/a; a = 1.0-b-c;

    if (a > -EPSILON && b > -EPSILON && c > -EPSILON) {
      VEC_INTER (Normal, a, Pnt->Normal, b, PntA->Normal, c, PntB->Normal);
      Real = VEC_DOT (Ray->Vector, Normal);
      if (Real > 0.0)
	return (FALSE);
      if (Hit) {
        Real = VEC_LEN (Normal); VEC_UNIT (Normal, Real);
        Ray->Distance = Distance; Hit->Point = Point; Hit->Normal = Normal;
	xyz2uv_geo_nappe (Geo, Hit, Fct);
      }
      return (TRUE);
    }
    return (FALSE);
  }
  return (FALSE);
}

