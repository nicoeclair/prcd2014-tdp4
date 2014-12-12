/*
 * lib.c - a library of vector operations, a random number generator, and
 *     object output routines.
 *
 * Version:  2.6 (8/28/89)
 * Author:  Eric Haines, 3D/Eye, Inc.
 */

#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "def.h"
#include "lib.h"


/*
 * Normalize the vector (X,Y,Z) so that X*X + Y*Y + Z*Z = 1.
 *
 * The normalization divisor is returned.  If the divisor is zero, no
 * normalization occurs.
 *
 */
double	lib_normalize_coord3( cvec )
COORD4	*cvec;
{
    double divisor;


    divisor = sqrt( (double)DOT_PRODUCT( (*cvec), (*cvec) ) ) ;

    if ( divisor != 0.0 ) {
	cvec->x /= divisor;
	cvec->y /= divisor;
	cvec->z /= divisor;
    }

    return( divisor );
}


/*
 * Set all matrix elements to zero.
 */
lib_zero_matrix( mx )
MATRIX	mx ;
{
    long    i, j ;


    for ( i = 0 ; i < 4 ; ++i ) {
	for ( j = 0 ; j < 4 ; ++j ) {
	    mx[i][j] = 0.0 ;
	}
    }
}


/*
 * Create identity matrix.
 */
lib_create_identity_matrix( mx )
MATRIX	mx ;
{
    long    i ;


    lib_zero_matrix( mx ) ;
    for ( i = 0 ; i < 4 ; ++i ) {
	mx[i][i] = 1.0 ;
    }
}


/*
 * Create a rotation matrix along the given axis by the given angle in radians.
 */
lib_create_rotate_matrix( mx, axis, angle )
MATRIX	mx ;
long	axis ;
double	angle ;
{
    double  cosine ;
    double  sine ;


    lib_zero_matrix( mx ) ;

    cosine = cos( (double)angle ) ;
    sine = sin( (double)angle ) ;

    switch ( axis ) {
	case X_AXIS:
	    mx[0][0] = 1.0 ;
	    mx[1][1] = mx[2][2] = cosine ;
	    mx[1][2] = sine ;
	    mx[2][1] = -sine ;
	    break ;
	case Y_AXIS:
	    mx[1][1] = 1.0 ;
	    mx[0][0] = mx[2][2] = cosine ;
	    mx[2][0] = sine ;
	    mx[0][2] = -sine ;
	    break ;
	case Z_AXIS:
	    mx[2][2] = 1.0 ;
	    mx[0][0] = mx[1][1] = cosine ;
	    mx[0][1] = sine ;
	    mx[1][0] = -sine ;
	    break ;
    }
    mx[3][3] = 1.0 ;
}


/*
 * Create a rotation matrix along the given axis by the given angle in radians.
 * The axis is a set of direction cosines.
 */
lib_create_axis_rotate_matrix( mx, rvec, angle )
MATRIX	mx ;
COORD4	*rvec ;
double	angle ;
{
    COORD4  axis ;
    double  cosine ;
    double  one_minus_cosine ;
    double  sine ;


    lib_zero_matrix( mx ) ;

    COPY_COORD( axis, (*rvec) ) ;

    cosine = cos( (double)angle ) ;
    sine = sin( (double)angle ) ;
    one_minus_cosine = 1.0 - cosine ;

    mx[0][0] = SQR(axis.x) + (1.0 - SQR(axis.x)) * cosine ;
    mx[0][1] = axis.x * axis.y * one_minus_cosine + axis.z * sine ;
    mx[0][2] = axis.x * axis.z * one_minus_cosine - axis.y * sine ;

    mx[1][0] = axis.x * axis.y * one_minus_cosine - axis.z * sine ;
    mx[1][1] = SQR(axis.y) + (1.0 - SQR(axis.y)) * cosine ;
    mx[1][2] = axis.y * axis.z * one_minus_cosine + axis.x * sine ;

    mx[2][0] = axis.x * axis.z * one_minus_cosine + axis.y * sine ;
    mx[2][1] = axis.y * axis.z * one_minus_cosine - axis.x * sine ;
    mx[2][2] = SQR(axis.z) + (1.0 - SQR(axis.z)) * cosine ;

    mx[3][3] = 1.0 ;
}


/*
 * Multiply a 4 element vector by a matrix.
 */
lib_transform_coord( vres, vec, mx )
COORD4	*vres ;
COORD4	*vec ;
MATRIX	mx ;
{
    vres->x =
	vec->x*mx[0][0] + vec->y*mx[1][0] + vec->z*mx[2][0] + vec->w*mx[3][0] ;
    vres->y =
	vec->x*mx[0][1] + vec->y*mx[1][1] + vec->z*mx[2][1] + vec->w*mx[3][1] ;
    vres->z =
	vec->x*mx[0][2] + vec->y*mx[1][2] + vec->z*mx[2][2] + vec->w*mx[3][2] ;
    vres->w =
	vec->x*mx[0][3] + vec->y*mx[1][3] + vec->z*mx[2][3] + vec->w*mx[3][3] ;
}


/*
 * Multiply two 4x4 matrices.
 */
lib_matrix_multiply( mxres, mx1, mx2 )
MATRIX	mxres ;
MATRIX	mx1 ;
MATRIX	mx2 ;
{
    long    i ;
    long    j ;


    for ( i = 0; i < 4; i++ ) {
	for ( j = 0; j < 4; j++ ) {
	    mxres[i][j] = mx1[i][0]*mx2[0][j] +
			  mx1[i][1]*mx2[1][j] +
			  mx1[i][2]*mx2[2][j] +
			  mx1[i][3]*mx2[3][j] ;
	}
    }
}


/*
 * Rotate a vector pointing towards the major-axis faces (i.e. the major-axis
 * component of the vector is defined as the largest value) 90 degrees to
 * another cube face.  Mod_face is a face number.
 *
 * If the routine is called six times, with mod_face=0..5, the vector will be
 * rotated to each face of a cube.  Rotations are:
 *     mod_face = 0 mod 3, +Z axis rotate
 *     mod_face = 1 mod 3, +X axis rotate
 *     mod_face = 2 mod 3, -Y axis rotate
 */
lib_rotate_cube_face( vec, major_axis, mod_face )
COORD4	*vec ;
long	major_axis ;
long	mod_face ;
{
    double  swap ;


    mod_face = (mod_face+major_axis) % 3 ;
    if ( mod_face == 0 ) {
	swap   = vec->x ;
	vec->x = -vec->y ;
	vec->y = swap ;
    }
    else if ( mod_face == 1 ) {
	swap   = vec->y ;
	vec->y = -vec->z ;
	vec->z = swap ;
    }
    else {
	swap   = vec->x ;
	vec->x = -vec->z ;
	vec->z = swap ;
    }
}


/*
 * Portable gaussian random number generator (from "Numerical Recipes", GASDEV)
 * Returns a uniform random deviate between 0.0 and 1.0.  'iseed' must be
 * less than M1 to avoid repetition, and less than (2**31-C1)/A1 [= 300718]
 * to avoid overflow.
 */
#define	M1	134456
#define	IA1	8121
#define	IC1	28411
#define	RM1	1.0/M1

double	lib_gauss_rand(iseed)
long	iseed ;
{
    double  fac ;
    long    ix1, ix2 ;
    double  r ;
    double  v1, v2 ;


    ix2 = iseed ;

    do {
	ix1 = (IC1+ix2*IA1) % M1 ;
	ix2 = (IC1+ix1*IA1) % M1 ;
	v1 = ix1 * 2.0 * RM1 - 1.0 ;
	v2 = ix2 * 2.0 * RM1 - 1.0 ;
	r = v1*v1 + v2*v2 ;
    } while ( r >= 1.0 ) ;

    fac = sqrt( (double)( -2.0 * log( (double)r ) / r ) ) ;
    return( v1 * fac ) ;
}




/* OUTPUT ROUTINES
 *
 * Files are output as lines of text.  For each entity, the first line
 * defines its type.  The rest of the first line and possibly other lines
 * contain further information about the entity.  Entities include:
 *
 * "v"  - viewing vectors and angles
 * "l"  - positional light location
 * "b"  - background color
 * "f"  - object material properties
 * "c"  - cone or cylinder primitive
 * "s"  - sphere primitive
 * "p"  - polygon primitive
 * "pp" - polygonal patch primitive
 */

/*
 * Output viewpoint location.  The parameters are:
 *   From:  the eye location.
 *   At:  a position to be at the center of the image.  A.k.a. "lookat"
 *   Up:  a vector defining which direction is up.
 *
 * Note that no assumptions are made about normalizing the data (e.g. the
 * from-at distance does not have to be 1).  Also, vectors are not
 * required to be perpendicular to each other.
 *
 * For all databases some viewing parameters are always the same:
 *
 *   Viewing angle is defined as from the center of top pixel row to bottom
 *     pixel row and left column to right column.
 *   Yon is "at infinity."
 *   Resolution is always 512 x 512.
 */
lib_output_viewpoint( from, at, up, angle, hither, resx, resy )
COORD4	*from ;
COORD4	*at ;
COORD4	*up ;
double	angle ;
double	hither ;
long	resx ;
long	resy ;
{
    printf( "v\n" ) ;
    printf( "from %g %g %g\n", from->x, from->y, from->z ) ;
    printf( "at %g %g %g\n", at->x, at->y, at->z ) ;
    printf( "up %g %g %g\n", up->x, up->y, up->z ) ;
    printf( "angle %g\n", angle ) ;
    printf( "hither %g\n", hither ) ;
    printf( "resolution %d %d\n", resx, resy ) ;
}


/*
 * Output light.  A light is defined by position.  All lights have the same
 * intensity.
 *
 */
lib_output_light( center_pt )
COORD4	*center_pt ;
{
    printf( "l %g %g %g\n", center_pt->x, center_pt->y, center_pt->z ) ;
}


/*
 * Output background color.  A color is simply RGB (monitor dependent, but
 * that's life).  The format is:
 *     "b" red green blue
 */
lib_output_background_color( color )
COORD4	*color ;
{
    printf( "b %g %g %g\n", color->x, color->y, color->z ) ;
}


/*
 * Output a color and shading parameters for the object in the format:
 *     "f" red green blue Kd Ks Shine T index_of_refraction
 *
 * Kd is the diffuse component, Ks the specular, Shine is the Phong cosine
 * power for highlights, T is transmittance (fraction of light passed per
 * unit).  0 <= Kd <= 1 and 0 <= Ks <= 1, though it is not required that
 * Kd + Ks == 1.
 *
 * The fill color is used to color the objects following it until a new color
 * is assigned or the file ends.
 */
lib_output_color( color, kd, ks, shine, t, i_of_r )
COORD4	*color ;
double	kd ;
double	ks ;
double	shine ;
double	t ;
double	i_of_r ;
{
    printf( "f %g %g %g %g %g %g %g %g\n", color->x, color->y, color->z,
						kd, ks, shine, t, i_of_r ) ;
}


/*
 * Output cylinder or cone.  A cylinder is defined as having a radius and an
 * axis defined by two points, which also define the top and bottom edge of the
 * cylinder.  A cone is defined similarly, the difference being that the apex
 * and base radii are different.  The apex radius is defined as being smaller
 * than the base radius.  Note that the surface exists without endcaps.
 *
 * If format=OUTPUT_CURVES, output the cylinder/cone in format:
 *     "c"
 *     base.x base.y base.z base_radius
 *     apex.x apex.y apex.z apex_radius
 *
 * If the format=OUTPUT_POLYGONS, the surface is polygonalized and output.
 * (4*OUTPUT_RESOLUTION) polygons are output as rectangles by
 * lib_output_polypatch.
 */
lib_output_cylcone( base_pt, apex_pt, format )
COORD4	*base_pt ;
COORD4	*apex_pt ;
long	format ;
{
    double  angle ;
    COORD4  axis ;
    COORD4  dir ;
    double  divisor ;
    COORD4  lip_norm[4], lip_pt[4] ;
    MATRIX  mx ;
    COORD4  norm_axis ;
    long    num_pol ;
    COORD4  start_norm, start_radius[4] ;


    if ( format == OUTPUT_CURVES ) {
	printf( "c\n" ) ;
	printf( "%g %g %g %g\n",
			    base_pt->x, base_pt->y, base_pt->z, base_pt->w ) ;
	printf( "%g %g %g %g\n",
			    apex_pt->x, apex_pt->y, apex_pt->z, apex_pt->w ) ;
    }
    else {
	SUB3_COORD( axis, (*apex_pt), (*base_pt) ) ;
	COPY_COORD( norm_axis, axis ) ;
	lib_normalize_coord3( &norm_axis ) ;

	dir.x = 0.0 ; dir.y = 0.0 ; dir.z = 1.0 ; dir.w = 0.0 ;
	CROSS( start_norm, axis, dir ) ;

	divisor = lib_normalize_coord3( &start_norm ) ;
	if ( ABSOLUTE( divisor ) < EPSILON ) {
	    dir.x = 1.0 ; dir.y = 0.0 ; dir.z = 0.0 ;
	    CROSS( start_norm, axis, dir ) ;
	    lib_normalize_coord3( &start_norm ) ;
	}

	start_radius[0].x = start_norm.x * base_pt->w ;
	start_radius[0].y = start_norm.y * base_pt->w ;
	start_radius[0].z = start_norm.z * base_pt->w ;
	start_radius[0].w = 0.0 ;
	ADD3_COORD( lip_pt[0], (*base_pt), start_radius[0] ) ;

	start_radius[1].x = start_norm.x * apex_pt->w ;
	start_radius[1].y = start_norm.y * apex_pt->w ;
	start_radius[1].z = start_norm.z * apex_pt->w ;
	start_radius[1].w = 0.0 ;
	ADD3_COORD( lip_pt[1], (*apex_pt), start_radius[1] ) ;

	start_norm.w = 0.0 ;
	COPY_COORD4( lip_norm[0], start_norm ) ;
	COPY_COORD4( lip_norm[1], start_norm ) ;

	for ( num_pol = 0 ; num_pol < 4*OUTPUT_RESOLUTION ; ++num_pol ) {
	    COPY_COORD4( lip_pt[3], lip_pt[0] ) ;
	    COPY_COORD4( lip_pt[2], lip_pt[1] ) ;
	    COPY_COORD4( lip_norm[3], lip_norm[0] ) ;
	    COPY_COORD4( lip_norm[2], lip_norm[1] ) ;

	    angle = 2.0 * PI *
		(double)( num_pol+1 ) / (double)( 4*OUTPUT_RESOLUTION ) ;
	    lib_create_axis_rotate_matrix( mx, &norm_axis, angle ) ;

	    lib_transform_coord( &lip_pt[0], &start_radius[0], mx ) ;
	    ADD2_COORD( lip_pt[0], (*base_pt) ) ;
	    lib_transform_coord( &lip_pt[1], &start_radius[1], mx ) ;
	    ADD2_COORD( lip_pt[1], (*apex_pt) ) ;

	    lib_transform_coord( &lip_norm[0], &start_norm, mx ) ;
	    COPY_COORD4( lip_norm[1], lip_norm[0] ) ;

	    lib_output_polypatch( 4, lip_pt, lip_norm ) ;
	}
    }
}


/*
 * Output sphere.  A sphere is defined by a radius and center position.
 *
 * If format=OUTPUT_CURVES, output the sphere in format:
 *     "s" center.x center.y center.z radius
 *
 * If the format=OUTPUT_POLYGONS, the sphere is polygonalized and output.
 * The sphere is polygonalized by splitting it into 6 faces (of a cube
 * projected onto the sphere) and dividing these faces by equally spaced
 * great circles.  OUTPUT_RESOLUTION affects the number of great circles.
 * (6*2*OUTPUT_RESOLUTION*OUTPUT_RESOLUTION) polygons are output as triangles
 * using lib_output_polypatch.
 */
lib_output_sphere( center_pt, format )
COORD4	*center_pt ;
long    format ;
{
    double  angle ;
    COORD4  edge_norm[3], edge_pt[3] ;
    long    num_face, num_edge, num_tri, num_vert ;
    COORD4  x_axis[OUTPUT_RESOLUTION+1], y_axis[OUTPUT_RESOLUTION+1] ;
    COORD4  pt[OUTPUT_RESOLUTION+1][OUTPUT_RESOLUTION+1] ;
    COORD4  mid_axis ;
    MATRIX  rot_mx ;
    long    u_pol, v_pol ;


    if ( format == OUTPUT_CURVES ) {
	printf( "s %g %g %g %g\n",
		    center_pt->x, center_pt->y, center_pt->z, center_pt->w ) ;
    }
    else {
	/* calculate axes used to find grid points */
	for ( num_edge = 0 ; num_edge <= OUTPUT_RESOLUTION ; ++num_edge ) {
 	    angle = (PI/4.0) * (2.0*(double)num_edge/OUTPUT_RESOLUTION - 1.0) ;
	    mid_axis.w = 0.0 ;

	    mid_axis.x = 1.0 ; mid_axis.y = 0.0 ; mid_axis.z = 0.0 ;
	    lib_create_rotate_matrix( rot_mx, Y_AXIS, angle ) ;
	    lib_transform_coord( &x_axis[num_edge], &mid_axis, rot_mx ) ;

	    mid_axis.x = 0.0 ; mid_axis.y = 1.0 ; mid_axis.z = 0.0 ;
	    lib_create_rotate_matrix( rot_mx, X_AXIS, angle ) ;
	    lib_transform_coord( &y_axis[num_edge], &mid_axis, rot_mx ) ;
	}

	/* set up grid of points on +Z sphere surface */
	for ( u_pol = 0 ; u_pol <= OUTPUT_RESOLUTION ; ++u_pol ) {
	    for ( v_pol = 0 ; v_pol <= OUTPUT_RESOLUTION ; ++v_pol ) {
		CROSS( pt[u_pol][v_pol], x_axis[u_pol], y_axis[v_pol] ) ;
		lib_normalize_coord3( &pt[u_pol][v_pol] ) ;
		pt[u_pol][v_pol].w = 1.0 ;
	    }
	}
	for ( num_face = 0 ; num_face < 6 ; ++num_face ) {
	    /* transform points to cube face */
	    for ( u_pol = 0 ; u_pol <= OUTPUT_RESOLUTION ; ++u_pol ) {
		for ( v_pol = 0 ; v_pol <= OUTPUT_RESOLUTION ; ++v_pol ) {
		    lib_rotate_cube_face( &pt[u_pol][v_pol]
					, Z_AXIS
					, num_face
					) ;
		}
	    }
	    /* output grid */
	    for ( u_pol = 0 ; u_pol < OUTPUT_RESOLUTION ; ++u_pol ) {
		for ( v_pol = 0 ; v_pol < OUTPUT_RESOLUTION ; ++v_pol ) {
		    for ( num_tri = 0 ; num_tri < 2 ; ++num_tri ) {
			for ( num_edge = 0 ; num_edge < 3 ; ++num_edge ) {
			    num_vert = (num_tri*2 + num_edge) % 4 ;
			    if ( num_vert == 0 ) {
				COPY_COORD4( edge_pt[num_edge],
					     pt[u_pol][v_pol] ) ;
			    }
			    else if ( num_vert == 1 ) {
				COPY_COORD4( edge_pt[num_edge],
					     pt[u_pol][v_pol+1] ) ;
			    }
			    else if ( num_vert == 2 ) {
				COPY_COORD4( edge_pt[num_edge],
					     pt[u_pol+1][v_pol+1] ) ;
			    }
			    else {
				COPY_COORD4( edge_pt[num_edge],
					     pt[u_pol+1][v_pol] ) ;
			    }
			    COPY_COORD4( edge_norm[num_edge],
					 edge_pt[num_edge] ) ;
			    edge_pt[num_edge].x =
					edge_pt[num_edge].x * center_pt->w +
							      center_pt->x ;
			    edge_pt[num_edge].y =
					edge_pt[num_edge].y * center_pt->w +
							      center_pt->y ;
			    edge_pt[num_edge].z =
					edge_pt[num_edge].z * center_pt->w +
							      center_pt->z ;

			}
			lib_output_polypatch( 3, edge_pt, edge_norm ) ;
		    }
		}
	    }
	}
    }
}


/*
 * Output polygon.  A polygon is defined by a set of vertices.  With these
 * databases, a polygon is defined to have all points coplanar.  A polygon has
 * only one side, with the order of the vertices being counterclockwise as you
 * face the polygon (right-handed coordinate system).
 *
 * The output format is always:
 *     "p" total_vertices
 *     vert1.x vert1.y vert1.z
 *     [etc. for total_vertices polygons]
 *
 */
lib_output_polygon( tot_vert, vert )
long	tot_vert ;
COORD4	*vert ;
{
    long    num_vert ;


    printf( "p %d\n", tot_vert ) ;

    for ( num_vert = 0 ; num_vert < tot_vert ; ++num_vert ) {
	printf( "%g %g %g\n", vert[num_vert].x
			    , vert[num_vert].y
			    , vert[num_vert].z
			    ) ;
    }
}


/*
 * Output polygonal patch.  A patch is defined by a set of vertices and their
 * normals.  With these databases, a patch is defined to have all points
 * coplanar.  A patch has only one side, with the order of the vertices being
 * counterclockwise as you face the patch (right-handed coordinate system).
 *
 * The output format is always:
 *     "pp" total_vertices
 *     vert1.x vert1.y vert1.z norm1.x norm1.y norm1.z
 *     [etc. for total_vertices polygonal patches]
 *
 */
lib_output_polypatch( tot_vert, vert, norm )
long	tot_vert ;
COORD4	*vert ;
COORD4	*norm ;
{
    long    num_vert ;


    printf( "pp %d\n", tot_vert ) ;

    for ( num_vert = 0 ; num_vert < tot_vert ; ++num_vert ) {
	printf( "%g %g %g %g %g %g\n", vert[num_vert].x
				     , vert[num_vert].y
				     , vert[num_vert].z
				     , norm[num_vert].x
				     , norm[num_vert].y
				     , norm[num_vert].z
				     ) ;
    }
}
