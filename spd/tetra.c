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
 */

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "def.h"
#include "lib.h"

#define	SIZE_FACTOR_DFLT		1
static long    SizeFactor = SIZE_FACTOR_DFLT;
static COORD4  CenterPoint = {0.0, 0.0, 0.0, 1.0};

main(argc,argv)
int argc ;
char *argv[] ;
{
    COORD4  back_color, tetra_color ;
/*    COORD4  center_pt, light ;*/
    COORD4  light;
    COORD4  from, at, up ;

    if (argc > 1)
	sscanf (argv[1], "%ld", &SizeFactor);
    if (argc > 2)
    {
	sscanf (argv[2], "%lf", &(CenterPoint.x));
	sscanf (argv[3], "%lf", &(CenterPoint.y));
	sscanf (argv[4], "%lf", &(CenterPoint.z));
	sscanf (argv[5], "%lf", &(CenterPoint.w));
    }

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
    SET_COORD( tetra_color, 1.0, 0.2, 0.2 ) ;
    lib_output_color( &tetra_color, 1.0, 0.0, 0.0, 0.0, 0.0 ) ;

    /* compute and output tetrahedral object */
/*    SET_COORD4( center_pt, 0.0, 0.0, 0.0, 10.0 ) ;*/
/*    create_tetra( SIZE_FACTOR, &center_pt ) ;*/
    create_tetra (SizeFactor, &CenterPoint);
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
	for ( num_vert = 0, x_dir = -1 ; x_dir <= 1 ; x_dir += 2 ) {
	    for ( y_dir = -1 ; y_dir <= 1 ; y_dir += 2 ) {
		for ( z_dir = -1 ; z_dir <= 1 ; z_dir += 2 ) {
		    if ( x_dir*y_dir*z_dir == 1 ) {
			obj_pt[num_vert].x =
					center->x + (double)x_dir * center->w ;
			obj_pt[num_vert].y =
					center->y + (double)y_dir * center->w ;
			obj_pt[num_vert].z =
					center->z + (double)z_dir * center->w ;
			++num_vert ;
		    }
		}
	    }
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
	    if ( num_face%2 == 1 ) {
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
	for ( x_dir = -1 ; x_dir <= 1 ; x_dir += 2 ) {
	    for ( y_dir = -1 ; y_dir <= 1 ; y_dir += 2 ) {
		for ( z_dir = -1 ; z_dir <= 1 ; z_dir += 2 ) {
		    if ( x_dir*y_dir*z_dir == 1 ) {
			sub_center.x =
				center->x + (double)x_dir * center->w / 2.0 ;
			sub_center.y =
				center->y + (double)y_dir * center->w / 2.0 ;
			sub_center.z =
				center->z + (double)z_dir * center->w / 2.0 ;
			sub_center.w = center->w / 2.0 ;

			create_tetra( depth-1, &sub_center ) ;
		    }
		}
	    }
	}
    }
}
