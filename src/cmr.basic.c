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
#include <math.h>

#include "cmr.h"

#include "macro.h"
#include "type.h"
#include "const.h"


typedef struct {
  VECTOR  Point;
  VECTOR  U, V, W;
  PAIR    Field, Cos, Sin;
} CMR_BASIC;

static CMR_BASIC  Cmr;


BOOL
file_cmr (FILE *File)
{
  REAL  Real;

  GET_VECTOR (Cmr.Point);
  GET_VECTOR (Cmr.W);
  VEC_UNIT (Cmr.W, Real);
  GET_VECTOR (Cmr.V);
  VEC_CROSS (Cmr.U, Cmr.W, Cmr.V);
  VEC_UNIT (Cmr.U, Real);
  VEC_CROSS (Cmr.V, Cmr.W, Cmr.U);
  VEC_UNIT (Cmr.V, Real);

  GET_PAIR (Cmr.Field);
  Cmr.Field.u *= 0.5*RAD;
  Cmr.Field.v *= 0.5*RAD;
  Cmr.Cos.u = cos (Cmr.Field.u);
  Cmr.Cos.v = cos (Cmr.Field.v);
  Cmr.Sin.u = sin (Cmr.Field.u);
  Cmr.Sin.v = sin (Cmr.Field.v);

  return (TRUE);
}


void
cmr (RAY *Ray, double u, double v)
{
  REAL  Real;

  u *= Cmr.Sin.u / Cmr.Cos.u;
  v *= Cmr.Sin.v / Cmr.Cos.v;
  VEC_LIN (Ray->Vector, Cmr.W, u, Cmr.U);
  VEC_LIN (Ray->Vector, Ray->Vector, v, Cmr.V);
  VEC_UNIT (Ray->Vector, Real);
  Ray->Point = Cmr.Point;
}
