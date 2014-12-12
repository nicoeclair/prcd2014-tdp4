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

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "scn.h"
#include "ett.h"
#include "bnd.h"
#include "lanceur.h"
#include "class.h"

typedef struct {
	COLOR	Color;         /* to have a more luminous scene */
	COLOR	Atm;           /* background color */
        REAL    Threshold;
	REAL    Rough;         /* to decide if a surface is a miror or not */
} SCN_IMAGE;

static SCN_IMAGE  Scn;

SCN *MainScn = (SCN *) &Scn;   /* public part of SCN structure */


BOOL file_scn (FILE *File)
{
  GET_REAL (Scn.Rough);
  GET_COLOR (Scn.Color);
  GET_COLOR (Scn.Atm);
  return (TRUE);
}

void get_scn (RAY *Ray, HIT *Hit)
{
  ETT    *Lgh;
  RAY    NewRay;
  HIT    NewHit;
  COLOR  Color;
  INDEX  Index;
  REAL   Real;
  VECTOR Vector;

  Color = Black;
  if (Hit->Ett->Opt.Light) {
    COL_MUL (Ray->Color, Ray->Color, Hit->Ett->Opt.Color);
    return;
  }

  if (Hit->Ett->Opt.MinRough < Scn.Rough && Ray->Level < NbrLevel) {
    Hit->u = RANDOM;
    Hit->v = RANDOM;
    NewRay.Level = Ray->Level + 1;
    NewRay.Point = Hit->Point;
    COL_MUL (NewRay.Color, Ray->Color, Hit->Ett->Opt.Color);
    Real = -2.0 * VEC_DOT (Ray->Vector, Hit->Normal);
    VEC_SCALE (NewRay.Vector, Real, Hit->Normal);
    VEC_INC (NewRay.Vector, Ray->Vector);
    VEC_UNIT (NewRay.Vector, Real);

    if (hit_bnd (&NewRay, &NewHit))
      get_scn (&NewRay, &NewHit);
    else
      COL_MUL (NewRay.Color, NewRay.Color, Scn.Atm);
    COL_INC (Color, NewRay.Color);
  }

  if (Hit->Ett->Opt.MaxRough > Scn.Rough) {
    for (Index = 0; Index < NbrLgh; Index++) {
      Hit->u = RANDOM; Hit->v = RANDOM;
      Lgh = MainLgh[Index];
      if (! ClassGeo[Lgh->Geo->Type].point (Lgh->Geo, Hit, &NewRay))
	continue;
      /* pour eviter les rayons en arriere */
      if (VEC_DOT (NewRay.Vector, Hit->Normal) < 0.0)
	continue;
      NewRay.Distance -= (Epsilon + Epsilon);
      NewRay.Color = Lgh->Opt.Color;
      VEC_SUB (Vector, NewRay.Vector, Ray->Vector);
      VEC_UNIT (Vector, Real);
      Real = VEC_DOT (Vector, Hit->Normal);
      if (Real < 0.0) {
	NewRay.Color = Black;
	return;
      }
      Real = (1.0 - Real) / Real / Hit->Ett->Opt.MaxRough;
      Real = 1.0 / (1.0 + Real);
      COL_MUL (NewRay.Color, NewRay.Color, Hit->Ett->Opt.Color);
      COL_MUL (NewRay.Color, NewRay.Color, Ray->Color);
      COL_SCALE (NewRay.Color, Real, NewRay.Color);
      if (COL_THRESHOLD (NewRay.Color))
	continue;
      if (hit_bnd (&NewRay, NULL))
	continue;
      COL_INC (Color, NewRay.Color);
    }
    COL_MUL (Ray->Color, Ray->Color, Hit->Ett->Opt.Color);
    COL_MUL (Ray->Color, Ray->Color, Scn.Color);
    COL_INC (Color, Ray->Color);
  }

  Ray->Color = Color;
}


void init_scn_image (void)
{
}

void exit_scn_image (void)
{
}

void scn (void)
{
  LST   *Lst, *LstLgh = NULL;
  ETT   *Ett;
  INDEX Index;
	
  init_scn_image ();

  for (Index = 0; Index < NbrEtt; Index++) {
    Ett = MainEtt[Index];
    if (! Ett->Opt.Light || COL_THRESHOLD (Ett->Opt.Color))
      continue;
    INIT_MEM (Lst, 1, LST); Lst->Data = Ett;
    Lst->Next = LstLgh; LstLgh = Lst; NbrLgh++;
  }

  INIT_MEM (MainLgh, NbrLgh, ETT*);
  for (Index = 0; (Lst = LstLgh) != NULL; Index++) {
    MainLgh[Index] = (ETT *) Lst->Data; LstLgh = Lst->Next; EXIT_MEM (Lst);
  }
  
  exit_scn_image ();
}
