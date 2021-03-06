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


#ifndef _BND_H_
#define _BND_H_


#include <stdio.h>

#include "type.h"
#include "type_spec.h"


typedef struct {
  VECTOR  Min, Max;
} BND;

extern BOOL file_bnd (FILE *File);
extern BOOL hit_bnd (RAY *Ray, HIT *Hit);
extern void bnd (void);

extern BND *MainBnd;

#endif
