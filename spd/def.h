/*
 * def.h contains some useful definitions for "C" programs.
 *
 * Version:  2.2 (11/17/87)
 * Author:  Eric Haines, 3D/Eye, Inc.
 */

#define EPSILON		5.0e-6

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef NULL
#define	NULL	0
#endif

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef PI
#define	PI	3.141592653589793
#endif


typedef	double		MATRIX[4][4] ;	/* row major form */

typedef	struct {
	double	x ;
	double	y ;
	double	z ;
	double	w ;
} COORD4, *COORD4_P ;


#define ABSOLUTE(A)		( (A) < 0 ? -(A) : (A) )
#define	FRACTION(A)		( (A) - (long)(A) )
#define	MAX(A,B)		( (A) > (B) ? (A) : (B) )
#define MAX3(A,B,C)		( MAX( MAX( A,B ), C ) )
#define	MIN(A,B)		( (A) < (B) ? (A) : (B) )
#define MIN3(A,B,C)		( MIN( MIN( A,B ), C ) )
#define SQR(A)			( (A) * (A) )

#define ADD2_COORD(r,a)		{ (r).x += (a).x; (r).y += (a).y;\
				  (r).z += (a).z; }
#define ADD3_COORD(r,a,b)	{ (r).x = (a).x + (b).x;\
				  (r).y = (a).y + (b).y;\
				  (r).z = (a).z + (b).z; }
#define COPY_COORD(r,a)		{ (r).x = (a).x; (r).y = (a).y; (r).z = (a).z;}
#define COPY_COORD4(r,a)	{ (r).x = (a).x; (r).y = (a).y; (r).z = (a).z;\
				  (r).w = (a).w; }
#define CROSS(r,a,b)		{ (r).x = (a).y * (b).z - (a).z * (b).y;\
				  (r).y = (a).z * (b).x - (a).x * (b).z;\
				  (r).z = (a).x * (b).y - (a).y * (b).x; }
#define DOT_PRODUCT(a,b)	( (a).x * (b).x +\
				  (a).y * (b).y +\
				  (a).z * (b).z )
#define SET_COORD(r,a,b,c)	{ (r).x = (a); (r).y = (b); (r).z = (c); }
#define SET_COORD4(r,a,b,c,d)	{ (r).x = (a); (r).y = (b); (r).z = (c);\
				  (r).w = (d); }
#define SUB2_COORD(r,a)		{ (r).x -= (a).x; (r).y -= (a).y;\
				  (r).z -= (a).z; }
#define SUB3_COORD(r,a,b)	{ (r).x = (a).x - (b).x;\
				  (r).y = (a).y - (b).y;\
				  (r).z = (a).z - (b).z; }
