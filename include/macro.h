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

#ifndef MACRO_H
#define MACRO_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "exit-if.h"

/*
** Generic macros
*/

#define	ABS(t)		((t) < 0 ? - (t) : (t))
#define ROUND(t)	((t) < 0 ? (int) ((t) - 0.5) : (int) ((t) + 0.5))
#define INTER(t,a,b)    ((a) + (t) * ((b) - (a)))

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#define MINMIN(a,b,c)	((a) < (b) ? ((a)<(c)?(a):(c)) : ((b)<(c)?(b):(c)))
#define MAXMAX(a,b,c)	((a) > (b) ? ((a)>(c)?(a):(c)) : ((b)>(c)?(b):(c)))

/*#define RANDOM		(TabRnd [NbrRnd++ & 4095])*/
/*#define RANDOM		(((REAL) random()) / 2147483648.0)*/
#define RANDOM		(((REAL) rand()) / 32767.0)
#define RAND(n)         (TabRnd [(n) & 4095])
#define HASH(i,j,k)     (23*(i) + 53*(j) + 83*(k))

/*
** Memory manipulation macros
*/

#define	INIT_MEM(P,N,T)	\
	if ((P = (T*) calloc (N, sizeof (T))) == NULL) { \
	  EXIT_NOIF ("Not enough memory"); \
	}

#define EXIT_MEM(P)	if (P != NULL) free (P)

/*
** File manipulation macros
*/
			
#define INIT_FILE(F,S,M) \
	if ((F = fopen (S, M)) == NULL) { \
	  fprintf (stderr, "File \"%s\" not found\n", S); \
	  EXIT_NOIF ("File not found"); \
	}
			
#define EXIT_FILE(F)	if (F != NULL) fclose (F)

/*
** List manipulation macros
*/

#define INIT_LST(N,L,P,T) { \
	  INIT_MEM (Lst, 1, LST); INIT_MEM (P, 1, T); P->Type = Type; \
	  strcpy (Lst->Name, Name); Lst->Data = (GENERIC *) P; \
	  Lst->Next = L; L = Lst; N++; \
	}

#define EXIT_LST(L)	while (Lst = L) { L = Lst->Next; EXIT_MEM (Lst); }

/*
** File read macros
*/

#define GET_NAME(L,P,T) { \
	  fscanf (File, "%s", String); \
	  if ((Lst = search (String, L)) == NULL) return (FALSE); \
	  P = (T*) Lst->Data; \
	}

#define GET_SEED(A)     { \
	  fscanf (File, " %c", &(A)); \
          if ((A) == '-') fscanf (File, "%c", &(A)); \
          else { ungetc ((A), File); (A) = 'P'; }\
	}

#define GET_REAL(A)	if (fscanf (File, "%f", &(A)) < 1) return (FALSE)

#define GET_PAIR(A)	if (fscanf (File, "%f %f", &(A).u, &(A).v) < 2) return (FALSE)

#define GET_VECTOR(A)	if (fscanf (File, "%f %f %f", &(A).x, &(A).y, &(A).z) < 3) return (FALSE)

#define GET_VECTOR_INT(A) if (fscanf (File, "%d %d %d", &(A).x, &(A).y, &(A).z)< 3) return (FALSE)

#define GET_COLOR(A)	if (fscanf (File, "%f %f %f", &(A).r, &(A).g, &(A).b) < 3) return (FALSE)

#define GET_INDEX(A)	if (fscanf (File, "%d", &(A)) < 1) return (FALSE)

#define GET_COUPLE(A)	if (fscanf (File, "%d %d", &(A).i, &(A).j) < 2) return (FALSE)

#define GET_TRIPLE(A)	if (fscanf (File, "%d %d %d", &(A).i, &(A).j, &(A).k) < 3) return (FALSE)

/*
** Color manipulation macros
*/

#define COL_MAKE(A,R,G,B) \
	((A).r = R, (A).g = G, (A).b = B)

#define COL_THRESHOLD(A) \
	((A).r < MainScn->Threshold && (A).g < MainScn->Threshold && (A).b < MainScn->Threshold)

#define COL_SCALE(R,a,A) \
	((R).r = a * (A).r, (R).g = a * (A).g, (R).b = a * (A).b)

#define	COL_INC(A,B) \
	((A).r += (B).r, (A).g += (B).g, (A).b += (B).b)

#define	COL_DEC(A,B) \
	((A).r -= (B).r, (A).g -= (B).g, (A).b -= (B).b)

#define COL_ADD(R,A,B) \
	((R).r = (A).r + (B).r, (R).g = (A).g + (B).g, (R).b = (A).b + (B).b)

#define COL_SUB(R,A,B) \
	((R).r = (A).r - (B).r, (R).g = (A).g - (B).g, (R).b = (A).b - (B).b)

#define COL_MUL(R,A,B) \
	((R).r = (A).r * (B).r, (R).g = (A).g * (B).g, (R).b = (A).b * (B).b)

#define COL_DIV(R,A,B) \
	((R).r = (A).r / (B).r, (R).g = (A).g / (B).g, (R).b = (A).b / (B).b)

#define COL_INV(R,a,A) \
	((R).r = 1.0 / (a-(A).r), (R).g = 1.0 / (a-(A).g), (R).b = 1.0 / (a-(A).b))

#define	COL_MIN(A,B) \
	((A).r = MIN((A).r,(B).r), (A).g = MIN((A).g,(B).g), (A).b = MIN((A).b,(B).b))

#define	COL_MAX(A,B) \
	((A).r = MAX((A).r,(B).r), (A).g = MAX((A).g,(B).g), (A).b = MAX((A).b,(B).b))

#define COL_INTER(R,t,A,B) \
	((R).r = (A).r + t * ((B).r - (A).r), \
	 (R).g = (A).g + t * ((B).g - (A).g), \
	 (R).b = (A).b + t * ((B).b - (A).b))

#define COL_UV(R,u,v,A,B,C,D) {\
	REAL a, b, c, d; \
        a = (1.0-(u))*(1.0-(v)); c = (u)*(v); \
        b = (u)*(1.0-(v)); d = (1.0-(u))*(v); \
        (R).r = a*(A).r + b*(B).r + c*(C).r + d*(D).r; \
        (R).g = a*(A).g + b*(B).g + c*(C).g + d*(D).g; \
        (R).b = a*(A).b + b*(B).b + c*(C).b + d*(D).b; \
       }

/*
** Vector manipulation macros
*/

#define VEC_MAKE(R,X,Y,Z)	((R).x = X, (R).y = Y, (R).z = Z)

#define	VEC_LEN(A)		(REAL) sqrt((A).x*(A).x+(A).y*(A).y+(A).z*(A).z)

#define	VEC_DOT(A,B)		(REAL) ((A).x*(B).x + (A).y*(B).y + (A).z*(B).z)

#define	VEC_NEG(A)		((A).x = -(A).x, (A).y = -(A).y, (A).z = -(A).z)

#define VEC_SCALE(R,k,A)	((R).x = k*(A).x, (R).y = k*(A).y, (R).z = k*(A).z)

#define	VEC_INC(A,B)		((A).x += (B).x, (A).y += (B).y, (A).z += (B).z)

#define	VEC_DEC(A,B)		((A).x -= (B).x, (A).y -= (B).y, (A).z -= (B).z)

#define	VEC_ADD(R,A,B) \
	((R).x = (A).x + (B).x, (R).y = (A).y + (B).y, (R).z = (A).z + (B).z)

#define	VEC_SUB(R,A,B) \
	((R).x = (A).x - (B).x, (R).y = (A).y - (B).y, (R).z = (A).z - (B).z)

#define	VEC_MUL(R,A,B) \
	((R).x = (A).x * (B).x, (R).y = (A).y * (B).y, (R).z = (A).z * (B).z)

#define	VEC_DIV(R,A,B) \
	((R).x = (A).x / (B).x, (R).y = (A).y / (B).y, (R).z = (A).z / (B).z)

#define VEC_UNIT(A,k) \
	(k = VEC_LEN (A), (A).x /= k, (A).y /= k, (A).z /= k)

#define	VEC_MIN(A,B) \
	((A).x = MIN((A).x,(B).x), (A).y = MIN((A).y,(B).y), (A).z = MIN((A).z,(B).z))

#define	VEC_MAX(A,B) \
	((A).x = MAX((A).x,(B).x), (A).y = MAX((A).y,(B).y), (A).z = MAX((A).z,(B).z))

#define	VEC_CROSS(R,A,B) \
	((R).x = (A).y * (B).z - (A).z * (B).y, \
	 (R).y = (A).z * (B).x - (A).x * (B).z, \
	 (R).z = (A).x * (B).y - (A).y * (B).x)

#define	VEC_LIN(R,A,k,B) \
	((R).x = (A).x + k * (B).x, \
	 (R).y = (A).y + k * (B).y, \
	 (R).z = (A).z + k * (B).z)

#define	VEC_INTER(R,a,A,b,B,c,C) \
	((R).x = a * (A).x + b * (B).x + c * (C).x, \
	 (R).y = a * (A).y + b * (B).y + c * (C).y, \
	 (R).z = a * (A).z + b * (B).z + c * (C).z)

#define VEC_UV(R,u,v,A,B,C,D) { \
        REAL a, b, c, d; \
        a = (1.0-(u))*(1.0-(v)); c = (u)*(v); \
        b = (u)*(1.0-(v)); d = (1.0-(u))*(v); \
        (R).x = a*(A).x + b*(B).x + c*(C).x + d*(D).x; \
        (R).y = a*(A).y + b*(B).y + c*(C).y + d*(D).y; \
        (R).z = a*(A).z + b*(B).z + c*(C).z + d*(D).z; \
       }


#endif
