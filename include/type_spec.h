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


#ifndef _TYPE_SPECIFIC_H_
#define _TYPE_SPECIFIC_H_

#include "type.h"
#include "ett.h"

/******************
 * Specific types *
 ******************/

/*-----*
 | Ray |
 *-----*/
typedef struct {
        VECTOR  Point, Vector;
        COLOR   Color;
        REAL    Distance;
        INDEX   Level;
} RAY;

/*--------------------*
 | Hit : Intersection |
 *--------------------*/
typedef struct {
        REAL    u, v;
        ETT     *Ett;
        VECTOR  Point, Normal;
} HIT;


#endif
