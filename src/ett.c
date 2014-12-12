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
#include <string.h>

#include "exit-if.h"

#include "macro.h"
#include "type.h"
#include "const.h"

#include "class.h"
#include "geo.h"

#include "ett.h"


INDEX  NbrEtt = 0;
ETT    **MainEtt = NULL;
INDEX  NbrLgh = 0;
ETT    **MainLgh = NULL;



ETT *
file_ett (FILE *File)
{
  ETT *Ett;
  STRING Light;
  STRING Geometry;
  INDEX  GeoType;

  INIT_MEM (Ett, 1, ETT);

  fscanf (File, "%s", Light);
  if (strcmp (Light, OPT_LIGHT) == 0)
    Ett->Opt.Light = TRUE;
  else if (strcmp (Light, OPT_NOLIGHT) == 0)
    Ett->Opt.Light = FALSE;
  else {
    EXIT_NOIF ("Error : light source property not defined");
  }
  GET_COLOR (Ett->Opt.Color);
  GET_REAL (Ett->Opt.MinRough);
  Ett->Opt.MaxRough = Ett->Opt.MinRough;

  fscanf (File, "%s", Geometry);
  for (GeoType = 0; (ClassGeo[GeoType].Text[0] != '\0') && 
	            strcmp (Geometry, ClassGeo[GeoType].Text);
       GeoType ++)
    /* empty */ ;
  EXIT_IF ((ClassGeo[GeoType].Text[0] == '\0'), "Unknown geometrical type");
  Ett->Geo = ClassGeo[GeoType].file (GeoType, File);

  return (Ett);
}
