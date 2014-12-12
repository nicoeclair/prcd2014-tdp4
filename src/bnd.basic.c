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


#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "class.h"
#include "lanceur.h"
#include "ett.h"
#include "bnd.h"

/*--------*
 | Kernel |
 *--------*/
typedef	struct krn {
	ETT		*Ett;
	void		*Info;
	struct krn	*Next;
} KRN;


typedef struct {
  VECTOR  Min, Max;
  KRN     *LstKrn;
} BND_BASIC;

static BND_BASIC  Bnd;

BND *MainBnd = (BND *) &Bnd;   /* public part of BND structure */

BOOL
file_bnd (FILE *File)
{
  return (TRUE);
}


BOOL hit_bnd (RAY *Ray, HIT *Hit)
{
  KRN             *Krn;
  PAIR            Bound;


  if (Hit) {
    Hit->Ett = NULL;
    Bound.u = Epsilon;
    Bound.v = INFINITY;
    Krn = Bnd.LstKrn;
    do {
      if (ClassGeo[Krn->Ett->Geo->Type].hit (Krn->Ett->Geo, Ray, Hit, &Bound, Krn->Info)) {
        Hit->Ett = Krn->Ett; Bound.v = Ray->Distance;
      }
    } while ((Krn = Krn->Next) != NULL);
    if (Hit->Ett) {
      return (TRUE);
    }
    return (FALSE);
  }
  else {
    Bound.u = Epsilon;
    Bound.v = Ray->Distance;
    Krn = Bnd.LstKrn;
    do {
      if (ClassGeo[Krn->Ett->Geo->Type].hit (Krn->Ett->Geo, Ray, NULL, &Bound, Krn->Info))
        return (TRUE);
    } while ((Krn = Krn->Next) != NULL);
    return (FALSE);
  }
}


void bnd (void)
{
  ETT             *Ett;
  KRN             *Krn;
  void            *Info;
  VECTOR          Min, Max;
  INDEX           Index, Kernel;

  Bnd.Min.x = Bnd.Min.y = Bnd.Min.z =  INFINITY;
  Bnd.Max.x = Bnd.Max.y = Bnd.Max.z = -INFINITY;
  for (Index = 0; Index < NbrEtt; Index++) {
    Ett = MainEtt[Index];
    Kernel = 0;
    VEC_MIN (Bnd.Min, Ett->Geo->Min);
    VEC_MAX (Bnd.Max, Ett->Geo->Max);
    do {
      ClassGeo[Ett->Geo->Type].binder (Ett->Geo, &Kernel, &Min, &Max, &Info);
      INIT_MEM (Krn, 1, KRN);
      Krn->Ett = Ett;
      Krn->Info = Info;
      Krn->Next = Bnd.LstKrn;
      Bnd.LstKrn = Krn;
    } while (Kernel);
  }
  assign_epsilon();
}
