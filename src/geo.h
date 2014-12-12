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

#ifndef _GEO_H_
#define _GEO_H_

#include <stdio.h>

#include "type.h"


#define PROT_FILE_GEO     BYTE, FILE *
#define PROT_HIT_GEO      GEO *, RAY *, HIT *, PAIR *, void *
#define PROT_POINT_GEO    GEO *, HIT *, RAY *
#define PROT_BINDER_GEO   GEO *, INDEX *, VECTOR *, VECTOR *, void **


/*-----------------------*
 | Geometry of an entity |
 *-----------------------*/
typedef struct {
  BYTE    Type;
  VECTOR  Min, Max;
} GEO;


#endif
