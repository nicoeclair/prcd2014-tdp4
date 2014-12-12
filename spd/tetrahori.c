/*
 * tetra.c - Create a tetrahedral pyramid.  This environment is based on the
 *     scene used by Glassner ("Space Subdivision for Fast Ray Tracing," IEEE
 *     CG&A, October 1984) and Kay & Kajiya ("Ray Tracing Complex Scenes,"
 *     SIGGRAPH '86 Proceedings) for testing their ray tracers.  No polygons
 *     are clipped.  One light source.
 *
 * Version:  2.2 (11/17/87)
 * Author:  Eric Haines, 3D/Eye, Inc.
 *
 * Note:  the view and light positions are the same (after transformation to
 *     a different set of world coordinates) as used by Kay & Kajiya,
 *     courtesy of Tim Kay.  For some reason, the number of shadow rays
 *     generated is different (Kay gets 34K, I get 46K).  One light source.
 *
 * SIZE_FACTOR determines the number of polygons output.
 *     Total triangular polygons = 4**SF
 *
 *     SIZE_FACTOR	# triangles
 *	    1		     4
 *	    2		    16
 *	    3		    64
 *
 *	    6		  4096
 *
 * modified bu Gilles Subrenat to have horizontal tetra.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "def.h"
#include "lib.h"


#define	SIZE_FACTOR_DFLT		1
static long    SizeFactor = SIZE_FACTOR_DFLT;
static COORD4  CenterPointDflt = {0.5, 0.2886751347,  0.2041241452, 1.0};
static COORD4  Vertices[4] = { {-0.5, -0.2886751347, -0.2041241452, 1.0},
                               { 0.5, -0.2886751347, -0.2041241452, 1.0},
                               { 0.0,  0.5773502693, -0.2041241452, 1.0},
                               { 0.0,  0.0         ,  0.6123724358, 1.0} };

int main(int argc, char *argv[])
{
    COORD4  back_color, tetra_color ;
    COORD4  CenterPoint, light;
    COORD4  from, at, up ;
    float   r=1.0, g=0.2, b=0.2, Rough=1.0;
    int     i;

    SET_COORD4 (CenterPoint, 0.0, 0.0, 0.0, 1.0) ;

    if (argc > 1)
	sscanf (argv[1], "%ld", &SizeFactor);
    if (argc > 2)
    {
	sscanf (argv[2], "%lf", &(CenterPoint.x));
	sscanf (argv[3], "%lf", &(CenterPoint.y));
	sscanf (argv[4], "%lf", &(CenterPoint.z));
	sscanf (argv[5], "%lf", &(CenterPoint.w));
    }
    if (argc > 6)
    {
	sscanf (argv[6], "%f", &r);
	sscanf (argv[7], "%f", &g);
	sscanf (argv[8], "%f", &b);
	sscanf (argv[9], "%f", &Rough);
    }

    CenterPoint.x = CenterPoint.x + CenterPointDflt.x * CenterPoint.w;
    CenterPoint.y = CenterPoint.y + CenterPointDflt.y * CenterPoint.w;
    CenterPoint.z = CenterPoint.z + CenterPointDflt.z * CenterPoint.w;

    /* output viewpoint */
    SET_COORD( from, 1.022846, -3.177154, -2.174512 ) ;
    SET_COORD( at, -0.004103, -0.004103, 0.216539 ) ;
    SET_COORD( up, -0.816497, -0.816497, 0.816497 ) ;
    lib_output_viewpoint( &from, &at, &up, 45.0, 1.0, 512, 512 ) ;

    /* output background color - UNC sky blue */
    SET_COORD( back_color, 0.078, 0.361, 0.753 ) ;
    lib_output_background_color( &back_color ) ;

    /* output light source */
    SET_COORD( light, 1.876066, -18.123936, -5.000422 ) ;
    lib_output_light( &light ) ;

    /* output tetrahedron color - red */
    SET_COORD( tetra_color, r, g, b ) ;
    lib_output_color( &tetra_color, Rough, 0.0, 0.0, 0.0, 0.0 ) ;

    /* compute and output tetrahedral object */
/*    SET_COORD4( center_pt, 0.0, 0.0, 0.0, 10.0 ) ;*/
/*    create_tetra( SIZE_FACTOR, &center_pt ) ;*/
    create_tetra (SizeFactor, &CenterPoint);

    return EXIT_SUCCESS;
}


/* Create tetrahedrons recursively */
create_tetra( depth, center )
long	depth ;
COORD4	*center ;
{
    long    num_face, num_vert ;
    COORD4  face_pt[3], obj_pt[4], sub_center ;
    long    swap, vert_ord[3] ;
    long    x_dir, y_dir, z_dir ;


    if ( depth <= 1 ) {
	/* Output tetrahedron */

	/* find opposite corners of a cube which form a tetrahedron */
	for (num_vert = 0 ; num_vert < 4 ; ++num_vert)
	{
	    obj_pt[num_vert].x = center->x + Vertices[num_vert].x * center->w;
	    obj_pt[num_vert].y = center->y + Vertices[num_vert].y * center->w;
	    obj_pt[num_vert].z = center->z + Vertices[num_vert].z * center->w;
	}

	/* find faces and output */
	for ( num_face = 0 ; num_face < 4 ; ++num_face ) {
	    /* output order:
	     *   face 0:  points 0 1 2
	     *   face 1:  points 3 2 1
	     *   face 2:  points 2 3 0
	     *   face 3:  points 1 0 3
	     */
	    for ( num_vert = 0 ; num_vert < 3 ; ++num_vert ) {
		vert_ord[num_vert] = (num_face + num_vert) % 4 ;
	    }
	    if ( num_face%2 == 0 ) {
		swap = vert_ord[0] ;
		vert_ord[0] = vert_ord[2] ;
		vert_ord[2] = swap ;
	    }

	    for ( num_vert = 0 ; num_vert < 3 ; ++num_vert ) {
		COPY_COORD( face_pt[num_vert], obj_pt[vert_ord[num_vert]] ) ;
	    }
	    lib_output_polygon( 3, face_pt ) ;
	}
    }

    else {
	/* Create sub-tetrahedra */

	/* find opposite corners of a cube to form sub-tetrahedra */
	for (num_vert = 0 ; num_vert < 4 ; ++num_vert)
	{
	    sub_center.x = center->x + Vertices[num_vert].x * center->w / 2.0 ;
	    sub_center.y = center->y + Vertices[num_vert].y * center->w / 2.0 ;
	    sub_center.z = center->z + Vertices[num_vert].z * center->w / 2.0 ;
	    sub_center.w = center->w / 2.0 ;

	    create_tetra( depth-1, &sub_center ) ;
	}
    }
}
