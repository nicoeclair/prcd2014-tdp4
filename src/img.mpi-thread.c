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
#include <assert.h>

#include "img.h"

#include "macro.h"
#include "const.h"
#include "type.h"
#include "type_spec.h"

#include "lanceur.h"
#include "scn.h"
#include "bnd.h"
#include "cmr.h"


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

struct TileElt
{
    int tile;
    struct TileElt *next;
};

struct TileQueue
{
    struct TileElt *head;
    struct TileElt *tail;
};

int
firstElement(struct TileQueue *q)
{
    assert(q != NULL);
    return q->head->tile;
}

int
isEmpty(struct TileQueue *q)
{
    return q->head == NULL;
}

void
addTile(struct TileQueue *q, int tile)
{
    assert(q != NULL);
    struct TileElt *e = malloc(sizeof(struct TileQueue));
    e->tile = tile;
    e->next = NULL;
    if (isEmpty(q))
    {
	q->head = e;
	q->tail = e;
    }
    else
    {
	q->tail->next = e;
	q->tail = e;
    }
}

void
pop(struct TileQueue *q)
{
    assert(q != NULL);
    if (isEmpty(q))
	return;
    
    free(q->head);
    q->head = q->head->next;
    if (q->head == NULL)
	q->tail = NULL;
}

void
printQueue(struct TileQueue *q)
{
    assert(q != NULL);
    struct TileElt *e = q->head;
    
    if (isEmpty(q))
    {
	puts("Empty queue");
	return;
    }
    
    while (e != NULL)
    {
	printf("%d - ", e->tile);
	e = e->next;
    }
    puts("");
}

void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR	 *TabColor, *Color;
  STRING Name;
  INDEX	 i, j;
  BYTE   Byte;
  struct TileQueue tiles = {NULL, NULL};
  
  printQueue(&tiles);
  addTile(&tiles, 1);
  printQueue(&tiles);
  printf("first elt = %d\n", firstElement(&tiles));
  addTile(&tiles, 7);
  printQueue(&tiles);
  pop(&tiles);
  pop(&tiles);
  printQueue(&tiles);
  
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
