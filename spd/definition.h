/********1*********2*********3*********4*********5*********6*********7********
 * Fichier :       DEFINITION.H                                              *
 *                                                                           *
 * Date creation :        30/09/1992                                         *
 *                                                                           *
 * Date modification :                                                       *
 * Nature modification :                                                     *
 *                                                                           *
 *****************************************************************************
 * Role : quelque definitions classiques                                     *
 *                                                                           *
 *****************************************************************************/


#ifndef DEFINITION_H
#define DEFINITION_H



/********************
 * Inclusion des .h *
 ********************/



/*****************************
 | quelques types classiques *
 *****************************/

typedef  unsigned short   BOOLEAN;
typedef  float            REAL;
typedef  unsigned char    uchar;
typedef  unsigned short   ushort;
typedef  unsigned int     uint;
typedef  unsigned long    ulong;


/******************************
 * quelques define classiques *
 ******************************/

#undef     TRUE
#define    TRUE         ((BOOLEAN) 1)

#undef     FALSE
#define    FALSE        ((BOOLEAN) 0)


/**********************************************
 * quelques defintions et macros mathematique *
 **********************************************/
#define PI		3.141592653589793
#define	RADIAN		1.745329251994329E-2
#define	INFINITY	1E35
#define	EPSILON		1E-5

#define ROUND(x)	((x) < 0 ? (int) ((x)-0.5) : (int) ((x)+0.5))
#define	ABS(x)		((x) < 0 ? -(x) : (x))
#define FABS(x)		((x) < 0.0 ? -(x) : (x))
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#define MINMIN(a,b,c)	((a) < (b) ? ((a)<(c)?(a):(c)) : ((b)<(c)?(b):(c)))
#define MAXMAX(a,b,c)	((a) > (b) ? ((a)>(c)?(a):(c)) : ((b)>(c)?(b):(c)))
#define SQR(a)          ((a) * (a))


/***********************************************
 * macro d'allocation et de liberation memoire *
 ***********************************************/
#define	ALLOC(n,t)      ((t*) calloc ((n),sizeof (t)))
#define FREE(p)         free(p)


#endif     /* DEFINITION_H */
