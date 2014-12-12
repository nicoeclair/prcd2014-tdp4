/*
 * lib.h - vector library definitions
 *
 * Version:  2.2 (11/17/87)
 * Author:  Eric Haines, 3D/Eye, Inc.
 */

#define	X_AXIS	0
#define	Y_AXIS	1
#define	Z_AXIS	2

/* Output library definitions */
#define OUTPUT_CURVES		0	/* true curve output */
#define OUTPUT_PATCHES		1	/* polygonal patches output */

#define OUTPUT_RESOLUTION	4	/* amount of polygonalization */

double	lib_normalize_coord3() ;
double	lib_gauss_rand() ;
