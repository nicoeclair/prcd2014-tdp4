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
#include <sys/time.h>

#include "getarg.h"
#include "exit-if.h"

#include "const.h"
#include "macro.h"
#include "type.h"

#include "lanceur.h"
#include "read_file.h"
#include "bnd.h"
#include "scn.h"
#include "img.h"

#ifndef NBR_LEVEL
#define NBR_LEVEL 16
#endif

static void usage (char *Name);


REAL     Epsilon;
VECTOR   Void = {0.0, 0.0, 0.0},
         Half = {0.5, 0.5, 0.5},
         Full = {1.0, 1.0, 1.0};
COLOR    White = {1.0, 1.0, 1.0},
         Black = {0.0, 0.0, 0.0};
REAL     Epsilon;
INDEX    NbrLevel = NBR_LEVEL;


static STRING  NameScn;


/*
** init_lumineur
*/
static void
init_lumineur (int argc, char **argv)
{
  char  Arg, *NameArg;

  if (argc == 1) usage (argv[0]);

  while ((Arg = getarg (argc, argv, &NameArg, "n.1!")) != 0)
    switch (Arg) {
      case '1' : strcpy (NameScn, NameArg); break;
      case 'n' : NbrLevel = atoi (NameArg); break;
      case '?' : EXIT_NOIF ("lanceur : Error in command line");
    }

  Epsilon = EPSILON;
}


double getTime(void){
  struct timeval tp;
  gettimeofday( &tp, NULL );
  return tp.tv_sec + 1e-6 * tp.tv_usec;
}


/*
** main
*/
int
main (int argc, char **argv)
{
  STRING Name;
  double start;
  double stop;

  init_lumineur (argc, argv);

  strcpy (Name, NameScn);
  strcat (Name, ".scn");

  start = getTime();
  read_scn (Name);
  stop = getTime();
  printf ("Temps de lecture  : %e seconde(s)\n", stop - start);

  start = getTime();
  bnd ();
  stop = getTime();
  printf ("Temps du binder   : %e seconde(s)\n", stop - start);

  start = getTime();
  scn ();
  stop = getTime();
  printf ("Temps de la scene : %e seconde(s)\n", stop - start);

  start = getTime();
  img (NameScn);
  stop = getTime();
  printf ("Temps de l'image  : %e seconde(s)\n", stop - start);

  exit (0);
}

/*
** assign Epsilon
*/
void
assign_epsilon (void)
{
    VECTOR Vector;

    VEC_SUB (Vector, MainBnd->Max, MainBnd->Min);
    Epsilon = MINMIN (Vector.x, Vector.y, Vector.z) * EPSILON/3.0;
    /*Epsilon = EPSILON;*/
}


static void
usage (char *Name) {
  fprintf (stderr, "Usage : %s [-n NbrLevel] basename\n", Name);
  exit (-1);
}
