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

#ifndef _SCN_H_
#define _SCN_H_


#include <stdio.h>

#include "type.h"
#include "type_spec.h"

typedef struct {
        COLOR   Color;         /* to have a more luminous scene */
        COLOR   Atm;           /* background color */
        REAL    Threshold;
} SCN;

extern BOOL file_scn (FILE *File);
extern void get_scn (RAY *Ray, HIT *Hit);
extern void scn (void);

extern SCN *MainScn;

#endif
