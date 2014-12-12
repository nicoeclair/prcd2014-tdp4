/********1*********2*********3*********4*********5*********6*********7********
 * Fichier :       MODELEUR.H                                               *
 *                                                                           *
 * Date creation :        04/03/1993                                         *
 *                                                                           *
 * Date modification :                                                       *
 * Nature modification :                                                     *
 *                                                                           *
 *****************************************************************************
 * Role : Definitions et types ayant trait au graphisme (patch, rayon, ...)  *
 *                                                                           *
 *****************************************************************************/


#ifndef MODELEUR_H
#define MODELEUR_H

/********************
 * Inclusion des .h *
 ********************/



/****************************************************************
 * macros diverses sur les voxels, tranches, vecteurs, couleurs *
 ****************************************************************/

/*----------------------------*
 |  Color manipulation macros |
 *----------------------------*/

#define MAKE_COL(A,R,G,B)      ((A).r = R, (A).g = G, (A).b = B)
#define COL_WORTH(A)	       ((A).r > Threshold || \
			        (A).g > Threshold || \
			        (A).b > Threshold)
#define COL_SCALE(R,a,A)       ((R).r = (a) * (A).r, \
			        (R).g = (a) * (A).g, \
			        (R).b = (a) * (A).b)
#define	COL_INC(A,B)	       ((A).r += (B).r, (A).g += (B).g, (A).b += (B).b)
#define	COL_DEC(A,B)	       ((A).r -= (B).r, (A).g -= (B).g, (A).b -= (B).b)
#define COL_ADD(R,A,B)	       ((R).r = (A).r + (B).r, \
			        (R).g = (A).g + (B).g, \
			        (R).b = (A).b + (B).b)
#define COL_SUB(R,A,B)	       ((R).r = (A).r - (B).r, \
			        (R).g = (A).g - (B).g, \
			        (R).b = (A).b - (B).b)
#define COL_MUL(R,A,B)	       ((R).r = (A).r * (B).r, \
			        (R).g = (A).g * (B).g, \
			        (R).b = (A).b * (B).b)
#define COL_DIV(R,A,B)	       ((R).r = (A).r / (B).r, \
			        (R).g = (A).g / (B).g, \
			        (R).b = (A).b / (B).b)
#define COL_INV(R)	       ((R).r = 1.0 / (1.0 - (R).r), \
			        (R).g = 1.0 / (1.0 - (R).g), \
			        (R).b = 1.0 / (1.0 - (R).b))
#define COL_PROD(R,a,A,B)      ((R).r = (a) * (A).r * (B).r, \
			        (R).g = (a) * (A).g * (B).g, \
			        (R).b = (a) * (A).b * (B).b)
#define COL_LIN(R,a,A,b,B)     ((R).r = (a) * (A).r + (b) * (B).r, \
			        (R).g = (a) * (A).g + (b) * (B).g, \
			        (R).b = (a) * (A).b + (b) * (B).b)
#define	COL_AVG(R,A,B,C,D)     ((R).r = 0.25 * ((A).r+(B).r+(C).r+(D).r), \
			        (R).g = 0.25 * ((A).g+(B).g+(C).g+(D).g), \
			        (R).b = 0.25 * ((A).b+(B).b+(C).b+(D).b))
#define	COL_INT(R,u,v,A,B,C,D) { REAL a, b, c, d; \
				a = (1.0-(u))*(1.0-(v)); c = (u)*(v); \
			        b = (u)*(1.0-(v)); d = (1.0-(u))*(v); \
				(R).r = a*(A).r + b*(B).r + c*(C).r + d*(D).r;\
				(R).g = a*(A).g + b*(B).g + c*(C).g + d*(D).g;\
				(R).b = a*(A).b + b*(B).b + c*(C).b + d*(D).b;\
			       }

/*-----------------------------*
 |  Vector manipulation macros |
 *-----------------------------*/

#define	MAKE_VEC(R,X,Y,Z)      ((R).x = X, (R).y = Y, (R).z = Z)
#define	VEC_LEN(A)             (REAL) sqrt((A).x*(A).x+(A).y*(A).y+(A).z*(A).z)
#define	VEC_NORM(A)            {REAL l; l = VEC_LEN (A);\
                               if (l != 0) {\
                                   (A).x /= l; (A).y /= l; (A).z /= l;}}
#define VEC_UNIT(A,a)          ((A).x /= (a), (A).y /= (a), (A).z /= (a))
#define	VEC_NEG(A)	       ((A).x = -(A).x, (A).y = -(A).y, (A).z = -(A).z)
#define VEC_SCALE(R,a,A)       ((R).x = (a)*(A).x, \
				(R).y = (a)*(A).y, \
				(R).z = (a)*(A).z)
#define	VEC_DOT(A,B)           (REAL) ((A).x*(B).x + (A).y*(B).y + (A).z*(B).z)
#define	VEC_INC(A,B)           ((A).x += (B).x, (A).y += (B).y, (A).z += (B).z)
#define	VEC_DEC(A,B)           ((A).x -= (B).x, (A).y -= (B).y, (A).z -= (B).z)
#define	VEC_ADD(R,A,B)         ((R).x = (A).x + (B).x, \
                                (R).y = (A).y + (B).y, \
                                (R).z = (A).z + (B).z)
#define	VEC_SUB(R,A,B)         ((R).x = (A).x - (B).x, \
                                (R).y = (A).y - (B).y, \
                                (R).z = (A).z - (B).z)
#define	VEC_MUL(R,A,B)         ((R).x = (A).x * (B).x, \
                                (R).y = (A).y * (B).y, \
                                (R).z = (A).z * (B).z)
#define	VEC_DIV(R,A,B)         ((R).x = (A).x / (B).x, \
                                (R).y = (A).y / (B).y, \
                                (R).z = (A).z / (B).z)
#define	VEC_HALF(R,A,B)        ((R).x = ((A).x - (B).x) / 2.0, \
                                (R).y = ((A).y - (B).y) / 2.0, \
                                (R).z = ((A).z - (B).z) / 2.0)
#define	VEC_CROSS(R,A,B)       ((R).x = (A).y * (B).z - (A).z * (B).y, \
                                (R).y = (A).z * (B).x - (A).x * (B).z, \
                                (R).z = (A).x * (B).y - (A).y * (B).x)
#define	VEC_LIN(R,a,A,b,B)     ((R).x = (a) * (A).x + (b) * (B).x, \
                                (R).y = (a) * (A).y + (b) * (B).y, \
                                (R).z = (a) * (A).z + (b) * (B).z)
#define	VEC_AVG(R,A,B,C,D)     ((R).x = 0.25 * ((A).x+(B).x+(C).x+(D).x), \
                                (R).y = 0.25 * ((A).y+(B).y+(C).y+(D).y), \
                                (R).z = 0.25 * ((A).z+(B).z+(C).z+(D).z))
#define	VEC_MIN(A,B)           ((A).x = MIN ((A).x, (B).x), \
                                (A).y = MIN ((A).y, (B).y), \
			        (A).z = MIN ((A).z, (B).z))
#define	VEC_MAX(A,B)           ((A).x = MAX ((A).x, (B).x), \
                                (A).y = MAX ((A).y, (B).y), \
                                (A).z = MAX ((A).z, (B).z))
#define VEC_EQUAL(A,B)         ((A).x-(B).x>-EPSILON && (A).x-(B).x<EPSILON &&\
                                (A).y-(B).y>-EPSILON && (A).y-(B).y<EPSILON &&\
                                (A).z-(B).z>-EPSILON && (A).z-(B).z<EPSILON)
#define	VEC_INT(R,u,v,A,B,C,D) { REAL a, b, c, d; \
                                a = (1.0-(u))*(1.0-(v)); c = (u)*(v); \
                                b = (u)*(1.0-(v)); d = (1.0-(u))*(v); \
                                (R).x = a*(A).x + b*(B).x + c*(C).x + d*(D).x;\
                                (R).y = a*(A).y + b*(B).y + c*(C).y + d*(D).y;\
                                (R).z = a*(A).z + b*(B).z + c*(C).z + d*(D).z;\
                               }

/*********************
 *  Types definition *
 *********************/

typedef	short	PACK;
typedef unsigned short	KIND;
typedef unsigned short  TYPE;

/*------------------------*
 | types vecteur et point |
 *------------------------*/
typedef	struct {
   REAL	x,         /* abscisse  */
        y,         /* ordonnee  */
        z;         /* hauteur   */
} VECTOR;

typedef	VECTOR	POINT;

/*---------------------------*
 | types doublons et triplet |
 *---------------------------*/
typedef	struct {
	REAL	i, j;
} PAIR;

typedef struct {
	int	i, j;
} COUPLE;

typedef struct {
	int	i, j, k;
} TRIPLE;


/*---------------*
 | type couleur  |
 |     r : red   |
 |     g : green |
 |     b : blue  |
 *---------------*/
typedef	struct {
	REAL	r, g, b;
} COLOR;


/*------------------------------------*
 | description geometrique d'un objet |
 *------------------------------------*/
typedef struct {
    int NbrPnt;      /* nbre de points constituant l'objet                   */
    int NbrFct;      /* nbre de facettes constituant l'objet                 */
} GEO;

/*---------------------------------------------------------------------------*
 | description optique d'un objet                                            |
 | d : Pourcentage de diffusion de la surface (0.0 <= d <= 1.0)              |
 | r : Pourcentage de reflexion de la surface (0.0 <= r <= 1.0)              |
 | t : Pourcentage de transmission de la surface (0.0 <= t <= 1.0)           |
 |            avec d+r+t = 1.0  theoriquement                                |
 | D : Couleur de la lumiere diffusee par la surface                         |
 | R : Couleur de la lumiere reflechie par la surface                        |
 | T : Couleur de la lumiere transmise par la surface                        |
 | E : Radiosite initiale de l'objet (different de (0.0,0.0,0.0) pour les    |
 |     sources lumineuses)                                                   |
 *---------------------------------------------------------------------------*/
typedef	struct {
	COLOR	E, D, R, T;
	REAL	d, r, t;
} OPT;


/*---------------------------------------*
 | description d'une facette geometrique |
 *---------------------------------------*/
typedef struct {
    int       i, j, k, l;  /* pointeur sur les quatres coins de la */
                           /* facette. eventuellement i = l        */
} FCT;


#endif    /* MODELEUR_H */
