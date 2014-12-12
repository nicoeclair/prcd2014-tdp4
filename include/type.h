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


#ifndef TYPE_H
#define TYPE_H

/*****************
 * Generic types *
 *****************/

typedef int		BOOL;

typedef unsigned char	BYTE;
typedef unsigned char   FLAGS;

typedef char		STRING[32];

typedef struct lst {
	void		*Data;
	struct lst	*Next;	
} LST;

typedef  int	INDEX;

typedef struct {
	INDEX	i, j;
} COUPLE;

typedef struct {
	INDEX	i, j, k;
} TRIPLE;

typedef float  REAL;

typedef	struct {
	REAL	u, v;
} PAIR;

typedef	struct {
	REAL	x, y, z;
} VECTOR;

typedef	struct {
	INDEX	x, y, z;
} VECTOR_INT;

typedef	struct {
	REAL	r, g, b;
} COLOR;


#endif
