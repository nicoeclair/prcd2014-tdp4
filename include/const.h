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

#ifndef CONST_H
#define CONST_H

/*
**  CONST.H :
**
**    Constants definition
*/

#define	TRUE		1
#define	FALSE		0

#define PI		(REAL) 3.141592653589793
#define	RAD		(REAL) 1.745329251994329E-2
#define	LOG		(REAL) -0.69314718
#ifdef INFINITY
#undef INFINITY
#endif
#define	INFINITY	(REAL) 1E35
#define	EPSILON		(REAL) 0.0001



#endif
