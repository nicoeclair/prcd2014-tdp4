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

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"
#include "mpi.h"

typedef struct {
  COUPLE  Pixel;
} IMG_BASIC;

static IMG_BASIC  Img;


BOOL
file_img (FILE *File)
{
  GET_COUPLE (Img.Pixel);
  return (TRUE);
}


static COLOR
pixel_basic (INDEX i, INDEX j)
{
  RAY    Ray;
  HIT    Hit;
  REAL   u, v;

  Ray.Level = 1;
  Ray.Color = White;
  u = 2.0 * i / Img.Pixel.i - 1.0;
  v = 2.0 * j / Img.Pixel.j - 1.0;
  cmr (&Ray, u, v);

  if (! hit_bnd (&Ray, &Hit))
    return (MainScn->Atm);
  get_scn (&Ray, &Hit);

  return (Ray.Color);
}


void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j;
  BYTE   Byte;
  int next_proc, prev_proc, nb_proc, myrank;
  MPI_Request rr, rs;
  MPI_Status st;
  char buffer[1024];
  
  MPI_Init(NULL, NULL);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  next_proc = (myrank + 1) % nb_proc;
  prev_proc = (myrank - 1 + nb_proc) % nb_proc;
  
  sprintf(buffer, "from %d to %d", myrank, next_proc);
  
  MPI_Isend(buffer, 1024, MPI_CHAR, next_proc, 32, MPI_COMM_WORLD, &rs);
  MPI_Irecv(buffer, 1024, MPI_CHAR, prev_proc, 32, MPI_COMM_WORLD, &rr);
  MPI_Wait(&rr, &st);
  
  printf("I am %d => %s\n", myrank, buffer);
  
  MPI_Finalize();
  
  strcpy (Name, FileNameImg);
  strcat (Name, ".ppm");
  INIT_FILE (FileImg, Name, "w");
  fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  INIT_MEM (TabColor, Img.Pixel.i, COLOR);

  for (j = 0; j < Img.Pixel.j; j++) {
    for (i = 0; i < Img.Pixel.i; i++) {
      TabColor [i] = pixel_basic (i, j);
    }
    for (i = 0, Color = TabColor; i < Img.Pixel.i; i++, Color++) {
      Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
      putc (Byte, FileImg);
      Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
      putc (Byte, FileImg);
      Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
      putc (Byte, FileImg);
    }
    fflush (FileImg);
  }

  EXIT_MEM (TabColor);
  EXIT_FILE (FileImg);
  
}
