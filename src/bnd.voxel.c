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

#include <stdio.h>
#include <math.h>


#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "class.h"
#include "lanceur.h"
#include "ett.h"
#include "bnd.h"


#define NEXT_END  0
#define NEXT_X    1
#define NEXT_Y    2
#define NEXT_Z    3

/*--------*
 | Kernel |
 *--------*/
typedef	struct krn {
	ETT		*Ett;
	void		*Info;
	struct krn	*Next;
} KRN;


typedef struct {
    VECTOR      Min, Max;
    VECTOR      Len, Inc;
    VECTOR_INT  Voxel;        /* Number of voxels for each axis */
    KRN	    **LstKrn;
} BND_VOXEL;


static BND_VOXEL  Bnd;

BND *MainBnd = (BND *) &Bnd;   /* public part of BND structure */




BOOL file_bnd (FILE *File)
{
    GET_VECTOR_INT (Bnd.Voxel);

    return (TRUE);
}

static BOOL intersect_bnd_voxel (RAY *Ray, HIT *Hit, PAIR *Bound, KRN *LstKrn)
{
    KRN   *Krn;

    Krn = LstKrn;

    /*-----------------------------------*
     | if no object then no intersection |
     *-----------------------------------*/
    if (! Krn)
	return (FALSE);

    /*--------------------*
     | intersection tries |
     *--------------------*/
    do {
        if (ClassGeo[Krn->Ett->Geo->Type].hit (Krn->Ett->Geo, Ray, Hit,
					       Bound, Krn->Info))
	{
	    if (Hit)
	    {
		Hit->Ett = Krn->Ett;
		Bound->v = Ray->Distance;
	    }
	    else
		return (TRUE);
	}
    } while ((Krn = Krn->Next) != NULL);

    if (Hit)
    {
	if (Hit->Ett) {
	    return (TRUE);
	}
    }

    return (FALSE);
}


BOOL hit_bnd (RAY *Ray, HIT *Hit)
{
    KRN		*Krn;
    VECTOR	Dist, IncDist;
    REAL        EntryDist, ExitDist;
    VECTOR_INT  IncVoxel;
    VECTOR_INT  NextVoxel;
    VECTOR_INT  NumVoxel;
    PAIR	Bound;
    BOOL        OriginInside;
    BOOL        Intersect;
    VECTOR      Point;
    VECTOR      PointOrigin;
    INDEX	Voxel, Next;
    REAL        Real;

    if (Hit)
    {
	Hit->Ett = NULL;
    }


    /*--------------------------------------------------*
     | is the ray origin outside the scene bounding box |
     | otherwise in which voxel it is                   |
     *--------------------------------------------------*/
    if ((Ray->Point.x < Bnd.Min.x) || (Ray->Point.x > Bnd.Max.x) ||
	(Ray->Point.y < Bnd.Min.y) || (Ray->Point.y > Bnd.Max.y) ||
	(Ray->Point.z < Bnd.Min.z) || (Ray->Point.z > Bnd.Max.z))
    {
	OriginInside = FALSE;
    }
    else
    {
	OriginInside = TRUE;
	EntryDist = 0.0;
    }

    /*--------------------------------------------*
     | Ray entry and ray exit of the bounding box |
     *--------------------------------------------*/
    if (Ray->Vector.x > EPSILON)
    {
	ExitDist = (Bnd.Max.x - Ray->Point.x - Epsilon) / Ray->Vector.x;
	if (! OriginInside)
	    EntryDist = (Bnd.Min.x - Ray->Point.x + Epsilon) /
		        Ray->Vector.x;
    }
    else if (Ray->Vector.x < -EPSILON)
    {
	ExitDist = (Bnd.Min.x - Ray->Point.x + Epsilon) / Ray->Vector.x;
	if (! OriginInside)
	    EntryDist = (Bnd.Max.x - Ray->Point.x - Epsilon) /
		        Ray->Vector.x;
    }
    else
    {
	ExitDist = INFINITY;
	if (! OriginInside)
	{
	    EntryDist = -INFINITY;
	    /*
	    ** if Ray parallel to YZplane and Ray out of Xlimits
	    */
	    if (Ray->Point.x < Bnd.Min.x + Epsilon)
		return (FALSE);
	    if (Ray->Point.x > Bnd.Max.x - Epsilon)
		return (FALSE);
	}
    }

    if (Ray->Vector.y > EPSILON)
    {
	Real = (Bnd.Max.y - Ray->Point.y - Epsilon) / Ray->Vector.y;
	ExitDist = MIN (ExitDist, Real);
	if (! OriginInside)
	{
	    Real = (Bnd.Min.y - Ray->Point.y + Epsilon) / Ray->Vector.y;
	    EntryDist = MAX (EntryDist, Real);
	}
    }
    else if (Ray->Vector.y < -EPSILON)
    {
	Real = (Bnd.Min.y - Ray->Point.y + Epsilon) / Ray->Vector.y;
	ExitDist = MIN (ExitDist, Real);
	if (! OriginInside)
	{
	    Real = (Bnd.Max.y - Ray->Point.y - Epsilon) / Ray->Vector.y;
	    EntryDist = MAX (EntryDist, Real);
	}
    }
    else
    {
	/* ExitDist = value computed by x */
	if (! OriginInside)
	{
	    /* EntryDist = value computed by x */
	    /*
	    ** if Ray parallel to XZplane and Ray out of ylimits
	    */
	    if (Ray->Point.y < Bnd.Min.y + Epsilon)
		return (FALSE);
	    if (Ray->Point.y > Bnd.Max.y - Epsilon)
		return (FALSE);
	}
    }

    if (Ray->Vector.z > EPSILON)
    {
	Real = (Bnd.Max.z - Ray->Point.z - Epsilon) / Ray->Vector.z;
	ExitDist = MIN (ExitDist, Real);
	if (! OriginInside)
	{
	    Real = (Bnd.Min.z - Ray->Point.z + Epsilon) / Ray->Vector.z;
	    EntryDist = MAX (EntryDist, Real);
	}
    }
    else if (Ray->Vector.z < -EPSILON)
    {
	Real = (Bnd.Min.z - Ray->Point.z + Epsilon) / Ray->Vector.z;
	ExitDist = MIN (ExitDist, Real);
	if (! OriginInside)
	{
	    Real = (Bnd.Max.z - Ray->Point.z - Epsilon) / Ray->Vector.z;
	    EntryDist = MAX (EntryDist, Real);
	}
    }
    else
    {
	/* ExitDist = value computed by x */
	if (! OriginInside)
	{
	    /* EntryDist = value computed by x */
	    /*
	    ** if Ray parallel to XYplane and Ray out of Zlimits
	    */
	    if (Ray->Point.z < Bnd.Min.z + Epsilon)
		return (FALSE);
	    if (Ray->Point.z > Bnd.Max.z - Epsilon)
		return (FALSE);
	}
    }

    if (ExitDist < Epsilon)
	return (FALSE);

    if (ExitDist < EntryDist + Epsilon)
	return (FALSE);

    /*
    ** if the ExitPoint is not in a face of the box bounding the
    ** scene, the ray does not hit the scene
    */
    VEC_LIN (Point, Ray->Point, ExitDist, Ray->Vector);
    if ((Point.x < Bnd.Min.x) || (Point.x > Bnd.Max.x))
	return (FALSE);
    if ((Point.y < Bnd.Min.y) || (Point.y > Bnd.Max.y))
	return (FALSE);
    if ((Point.z < Bnd.Min.z) || (Point.z > Bnd.Max.z))
	return (FALSE);

    /*
    ** compute the entry point
    */
    if (OriginInside)
    {
	PointOrigin = Ray->Point;
	Bound.u = Epsilon + Epsilon;
	Bound.v = Epsilon + Epsilon;
    }
    else
    {
	VEC_LIN (PointOrigin, Ray->Point, EntryDist, Ray->Vector);
	Bound.u = EntryDist;
	Bound.v = EntryDist;
    }

    /*
    ** Voxel of the origin point
    */
    NumVoxel.x = (PointOrigin.x - Bnd.Min.x) / Bnd.Len.x * Bnd.Voxel.x;
    NumVoxel.y = (PointOrigin.y - Bnd.Min.y) / Bnd.Len.y * Bnd.Voxel.y;
    NumVoxel.z = (PointOrigin.z - Bnd.Min.z) / Bnd.Len.z * Bnd.Voxel.z;

    /*--------------*
     | Progressions |
     *--------------*/
    if (Ray->Vector.x > EPSILON)
    {
	NextVoxel.x = 1;
	IncVoxel.x = 1;
	IncDist.x = Bnd.Inc.x / Ray->Vector.x;
	Dist.x = (Bnd.Min.x + Bnd.Inc.x * NumVoxel.x + Bnd.Inc.x -
		  Ray->Point.x) / Ray->Vector.x;
	if (Dist.x < Epsilon)
	{
	    Dist.x += IncDist.x;
	    NumVoxel.x ++;
	}
    }
    else if (Ray->Vector.x < -EPSILON)
    {
	NextVoxel.x = -1;
	IncVoxel.x = -1;
	IncDist.x = -Bnd.Inc.x / Ray->Vector.x;
	Dist.x = (Bnd.Min.x + Bnd.Inc.x * NumVoxel.x - Ray->Point.x) /
	    Ray->Vector.x;
	if (Dist.x < Epsilon)
	{
	    Dist.x += IncDist.x;
	    NumVoxel.x --;
	}
    }
    else
    {
	Ray->Vector.x = 0.0;
	NextVoxel.x = 0;
	IncVoxel.x = 0;
	IncDist.x = 0;
	Dist.x = INFINITY;
    }

    if (Ray->Vector.y > EPSILON)
    {
	NextVoxel.y = 1;
	IncVoxel.y = Bnd.Voxel.x;
	IncDist.y = Bnd.Inc.y / Ray->Vector.y;
	Dist.y = (Bnd.Min.y + Bnd.Inc.y * NumVoxel.y + Bnd.Inc.y -
		  Ray->Point.y) / Ray->Vector.y;
	if (Dist.y < Epsilon)
	{
	    Dist.y += IncDist.y;
	    NumVoxel.y ++;
	}
    }
    else if (Ray->Vector.y < -EPSILON)
    {
	NextVoxel.y = -1;
	IncVoxel.y = -Bnd.Voxel.x;
	IncDist.y = -Bnd.Inc.y / Ray->Vector.y;
	Dist.y = (Bnd.Min.y + Bnd.Inc.y * NumVoxel.y - Ray->Point.y) /
	    Ray->Vector.y;
	if (Dist.y < Epsilon)
	{
	    Dist.y += IncDist.y;
	    NumVoxel.y --;
	}
    }
    else
    {
	Ray->Vector.y = 0.0;
	NextVoxel.y = 0;
	IncVoxel.y = 0;
	IncDist.y = 0;
	Dist.y = INFINITY;
	/* ExitDist = value computed by x */
    }

    if (Ray->Vector.z > EPSILON)
    {
	NextVoxel.z = 1;
	IncVoxel.z = Bnd.Voxel.x * Bnd.Voxel.y;
	IncDist.z = Bnd.Inc.z / Ray->Vector.z;
	Dist.z = (Bnd.Min.z + Bnd.Inc.z * NumVoxel.z + Bnd.Inc.z -
		  Ray->Point.z) / Ray->Vector.z;
	if (Dist.z < Epsilon)
	{
	    Dist.z += IncDist.z;
	    NumVoxel.z ++;
	}
    }
    else if (Ray->Vector.z < -EPSILON)
    {
	NextVoxel.z = -1;
	IncVoxel.z = -Bnd.Voxel.x * Bnd.Voxel.y;
	IncDist.z = -Bnd.Inc.z / Ray->Vector.z;
	Dist.z = (Bnd.Min.z + Bnd.Inc.z * NumVoxel.z - Ray->Point.z) /
	    Ray->Vector.z;
	if (Dist.z < Epsilon)
	{
	    Dist.z += IncDist.z;
	    NumVoxel.z --;
	}
    }
    else
    {
	Ray->Vector.z = 0.0;
	NextVoxel.z = 0;
	IncVoxel.z = 0;
	IncDist.z = 0;
	Dist.z = INFINITY;
    }

    /*
    ** if Hit==NULL, stop the search when the ray runs a distance
    ** equals to Ray->Distance;
    */
    if (Hit == NULL)
    {
	ExitDist = MIN(ExitDist, Ray->Distance);
    }

    Voxel = NumVoxel.x + Bnd.Voxel.x * (NumVoxel.y + Bnd.Voxel.y * NumVoxel.z);

    /*-----------------*
     | Ray progression |
     *-----------------*/
    do {
	Krn = Bnd.LstKrn[Voxel];

	Next = Dist.x < Dist.y ? Dist.x < Dist.z ? NEXT_X : NEXT_Z
			       : Dist.y < Dist.z ? NEXT_Y : NEXT_Z;

	switch (Next)
	{
	    case NEXT_X: Bound.u = MAX (Bound.v, Epsilon+Epsilon);
	                 Bound.v = Dist.x;
	                 break;
	    case NEXT_Y: Bound.u = MAX (Bound.v, Epsilon+Epsilon);
	                 Bound.v = Dist.y;
	                 break;
	    case NEXT_Z: Bound.u = MAX (Bound.v, Epsilon+Epsilon);
	                 Bound.v = Dist.z;
	                 break;
	}

        if (Bound.v > ExitDist) 
	{
	    Bound.v = ExitDist;
	    Next = NEXT_END;
	}

        /*
	** intersection computation
	*/
        Bound.u -= Epsilon;
        Bound.v += Epsilon;

	Intersect = intersect_bnd_voxel (Ray, Hit, &Bound, Krn);

        Bound.u += Epsilon;
        Bound.v -= Epsilon;
        if (Intersect)
	    return (TRUE);

        switch (Next)
	{
	    case NEXT_X: NumVoxel.x += NextVoxel.x;
	                 Voxel += IncVoxel.x;
	                 Dist.x += IncDist.x;
	                 break;
	    case NEXT_Y: NumVoxel.y += NextVoxel.y;
	                 Voxel += IncVoxel.y;
	                 Dist.y += IncDist.y;
	                 break;
	    case NEXT_Z: NumVoxel.z += NextVoxel.z;
	                 Voxel += IncVoxel.z;
	                 Dist.z += IncDist.z;
	                 break;
	}
    } while (Next != NEXT_END);

    return (FALSE);
}





void bnd (void)
{
    ETT		*Ett;
    KRN		*Krn;
    void	*Info;
    VECTOR	Vector, Min, Max;
    TRIPLE	MinVoxel, MaxVoxel;
    INDEX	Voxel, Index, Kernel, i, j, k;


    Bnd.Min.x = Bnd.Min.y = Bnd.Min.z =  INFINITY;
    Bnd.Max.x = Bnd.Max.y = Bnd.Max.z = -INFINITY;
    for (Index = 0; Index < NbrEtt; Index++) {
	Ett = MainEtt[Index];
	VEC_MIN (Bnd.Min, Ett->Geo->Min);
	VEC_MAX (Bnd.Max, Ett->Geo->Max);
    }
    VEC_SUB (Vector, Bnd.Max, Bnd.Min);
    VEC_SCALE (Vector, EPSILON, Vector);
    VEC_INC (Bnd.Max, Vector);
    VEC_DEC (Bnd.Min, Vector);
    VEC_SUB (Bnd.Len, Bnd.Max, Bnd.Min);
    VEC_DIV (Bnd.Inc, Bnd.Len, Bnd.Voxel);
    assign_epsilon();

    Voxel = Bnd.Voxel.x * Bnd.Voxel.y * Bnd.Voxel.z;
    INIT_MEM (Bnd.LstKrn, Voxel, KRN *);

    for (Index = 0; Index < NbrEtt; Index++) {
	Ett = MainEtt[Index];
	Kernel = 0;
	do {
	    ClassGeo[Ett->Geo->Type].binder (Ett->Geo, &Kernel, &Min, &Max, &Info);
	    MinVoxel.i = (Min.x - Bnd.Min.x) / Bnd.Len.x * Bnd.Voxel.x;
	    MinVoxel.j = (Min.y - Bnd.Min.y) / Bnd.Len.y * Bnd.Voxel.y;
	    MinVoxel.k = (Min.z - Bnd.Min.z) / Bnd.Len.z * Bnd.Voxel.z;
	    MaxVoxel.i = (Max.x - Bnd.Min.x) / Bnd.Len.x * Bnd.Voxel.x;
	    MaxVoxel.j = (Max.y - Bnd.Min.y) / Bnd.Len.y * Bnd.Voxel.y;
	    MaxVoxel.k = (Max.z - Bnd.Min.z) / Bnd.Len.z * Bnd.Voxel.z;
	    for (k = MinVoxel.k; k <= MaxVoxel.k; k++) 
		for (j = MinVoxel.j; j <= MaxVoxel.j; j++) 
		    for (i = MinVoxel.i; i <= MaxVoxel.i; i++) {
			Voxel = i + Bnd.Voxel.x * (j + Bnd.Voxel.y * k);
			INIT_MEM (Krn, 1, KRN);
			Krn->Ett = Ett; Krn->Info = Info; 
			Krn->Next = Bnd.LstKrn[Voxel];
			Bnd.LstKrn[Voxel] = Krn;
		    }
	} while (Kernel);
    }
}
