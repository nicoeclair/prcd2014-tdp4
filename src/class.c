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

#include "class.h"


/****************************
 * CLASS GEO (for Geometry) *
 ****************************/
#include "geo.h"
#include "geo.nappe.h"
#include "geo.quadric.h"
#include "geo.point.h"

CLASS_GEO ClassGeo[] = {
  {"NAPPE",  file_geo_nappe,  hit_geo_nappe,  point_geo_nappe,  binder_geo_nappe},
  {"SPHERE", file_geo_sphere, hit_geo_sphere, point_geo_sphere, binder_geo_sphere},
  {"POINT",  file_geo_point,  hit_geo_point,  point_geo_point,  binder_geo_point},
  {"", NULL, NULL, NULL, NULL}
};
