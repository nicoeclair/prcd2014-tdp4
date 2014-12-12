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


#ifndef _CMR_H_
#define _CMR_H_


#include <stdio.h>

#include "type.h"
#include "type_spec.h"

extern BOOL file_cmr (FILE *File);
extern void cmr (RAY *Ray, double u, double v);


#endif
