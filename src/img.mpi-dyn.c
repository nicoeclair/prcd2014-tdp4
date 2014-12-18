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
#include "mpi.h"
#include <pthread.h>


#define TILE_SIZE 8
#define NB_THREADS 4
 pthread_mutex_t mutex;
 MPI_Datatype MPI_COLOR;
 int Ci;
 int Cj;
 int terminated = 0;
 int looking_for_work = 0;
 int work_asked = 0;

 enum tag_e {WORK_ASK, WORK_SEND, TERMINATE, TILE_TAG_INDEX};

 int min(int a, int b){
  return (a<b)?a:b;
}

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


// first column of rank-th tile
int rank_i(int rank, int Ci)
{
  return (rank % Ci) * TILE_SIZE;
}

// first line of rank-th tile 
int rank_j(int rank, int Cj)
{
  return rank/Cj * TILE_SIZE;
}

int
chinese_remainder_bound(int rank, int q, int C)
{
  return min((rank+1)*q - 1, C - 1);
}

int
chinese_remainder_value(int k, int N, int C)
{
  return (k * N) % C;
}


int tile_fill(struct TileQueue *tiles)
{
  INDEX i,j;
  COLOR *TileColor;
  INIT_MEM(TileColor, TILE_SIZE * TILE_SIZE, COLOR);
  
  while(!terminated){
    pthread_mutex_lock(&mutex);
    if (!isEmpty(tiles)) {
      int current_tile = firstElement(tiles);
      pop(tiles);
      pthread_mutex_unlock(&mutex);

      // assigning first and final index of tile
      int j_begin = rank_j(current_tile, Cj);
      int j_end = min(j_begin + TILE_SIZE, Img.Pixel.j);
      int i_begin = rank_i(current_tile, Ci);
      int i_end = min(i_begin + TILE_SIZE, Img.Pixel.i);
      for (j = j_begin; j < j_end ; j++) {
        for (i = i_begin ; i < i_end; i++) {
          TileColor [(j-j_begin) * TILE_SIZE + (i-i_begin)] = pixel_basic (i, j);
        }
      }
      MPI_Send(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, 0, current_tile + TILE_TAG_INDEX, MPI_COMM_WORLD);
      pthread_mutex_lock(&mutex);
    } else {
      looking_for_work = 1;
    }
    pthread_mutex_unlock(&mutex);
  }

  return 0;
}

void
img (const char *FileNameImg)
{
  FILE   *FileImg;   
  COLOR  *TabColor, *Color, *TileColor;
  STRING Name;
  INDEX  i, j, k,  rank, P;
  BYTE   Byte;
  int N = 18988, err, provided;
  int next_proc;
  MPI_Request rs;

  MPI_Status status;
  MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);
  next_proc = (rank + 1) % P;
  if (next_proc == 0) next_proc++;
  P--;
  
  // sprintf(buffer, "from %d to %d", rank, next_proc);
  // MPI_Isend(buffer, 1024, MPI_CHAR, next_proc, 32, MPI_COMM_WORLD, &rs);
  // MPI_Irecv(buffer, 1024, MPI_CHAR, prev_proc, 32, MPI_COMM_WORLD, &rr);
  // MPI_Wait(&rr, &status);
  // printf("I am %d => %s\n", rank, buffer);

  MPI_Type_vector(1, 3, 0, MPI_FLOAT, &MPI_COLOR);
  MPI_Type_commit(&MPI_COLOR);

  if (rank == 0) {
    strcpy (Name, FileNameImg);
    strcat (Name, ".ppm");
    INIT_FILE (FileImg, Name, "w");
    fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
  }

  // number of tiles
  Ci = Img.Pixel.i / TILE_SIZE + (Img.Pixel.i % TILE_SIZE?1:0); // number of tiles in dimension i
  Cj = Img.Pixel.j / TILE_SIZE + (Img.Pixel.i % TILE_SIZE?1:0);  // number of tiles in dimension j
  int C = Ci * Cj;
  int q = (C+P-1)/P;
  int size = Img.Pixel.i * Img.Pixel.j ;
  N = C/2+1;

  // buffer for each tile
  INIT_MEM (TileColor, TILE_SIZE * TILE_SIZE, COLOR);
  if (rank != 0) {
    struct TileQueue tiles = {NULL, NULL};

    for (k = (rank-1) * q; k <= chinese_remainder_bound((rank-1), q, C); k++){
    // pushing tile in queue of current process
      addTile(&tiles,chinese_remainder_value(k, N, C));
    }

    pthread_mutex_init(&mutex,NULL);
    pthread_t tid[NB_THREADS];
    for (i = 0; i < NB_THREADS; i++){
      err = pthread_create(&(tid[i]), NULL, (void*)tile_fill, (void*)&tiles);
      if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    }
    while (!terminated)
    {
      int flag = 0, msg;
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
      if (flag) {
        // printf("Source %d\t Tag %d\n", status.MPI_SOURCE, status.MPI_TAG);
        MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
        // printf("Goodbye\n");
        if (rank == 3) printf("rank 3 received %d\n", status.MPI_TAG);
        switch (status.MPI_TAG){
          case TERMINATE:
          // printf("TEST %d: %d\n",rank , terminated);
          MPI_Isend(&msg, 1, MPI_INT, next_proc, TERMINATE, MPI_COMM_WORLD, &rs);
          terminated = 1;
            break;
          case WORK_ASK:
          pthread_mutex_lock(&mutex);
          if(!isEmpty(&tiles)){
            int tile = firstElement(&tiles);
            pop(&tiles);
            pthread_mutex_unlock(&mutex);
            MPI_Isend(&tile, 1, MPI_INT, status.MPI_SOURCE, WORK_SEND, MPI_COMM_WORLD, &rs);
          }
          else {
            pthread_mutex_unlock(&mutex);
            if (msg == rank){
              MPI_Isend(&msg, 1, MPI_INT, next_proc, TERMINATE, MPI_COMM_WORLD, &rs);
              terminated = 1;
            } else { 
              MPI_Isend(&msg, 1, MPI_INT, next_proc, WORK_ASK, MPI_COMM_WORLD, &rs);
            }
          }
          break;
          case WORK_SEND: 
          pthread_mutex_lock(&mutex);
          addTile(&tiles, msg);
          pthread_mutex_unlock(&mutex);
          work_asked = 0;
          looking_for_work = 0;
          break;
          default: 
          fprintf(stderr, "Err: Unknown message: %d, with tag %d\n", msg,status.MPI_TAG); 
          break;
        }
      }
      if (looking_for_work && !work_asked){
        MPI_Isend(&rank, 1, MPI_INT, next_proc, WORK_ASK, MPI_COMM_WORLD, &rs);
        work_asked = 1;
      }

    }
    for (i = 0; i < NB_THREADS; i++){
      pthread_join(tid[i],NULL);
    }

  // tile_fill(&tiles);
    pthread_mutex_destroy(&mutex);
  }
  if (rank == 0){ // process 0 gathers all the tiles
    // final image buffer
    INIT_MEM (TabColor, size, COLOR);

    for (j = 0; j < TILE_SIZE; j++) {
      memcpy(&TabColor[j * Img.Pixel.i ],&TileColor[j * TILE_SIZE],TILE_SIZE * sizeof(COLOR));
    }
    // Receive tiles from other procs
    for (i = 0; i < C ; i++){
      MPI_Recv(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      int current_tile = status.MPI_TAG - TILE_TAG_INDEX;
      int j_begin = rank_j(current_tile,Cj);
      int index_begin = rank_i(current_tile,Ci) + j_begin * Img.Pixel.i;
      for (j = 0; j < TILE_SIZE && j_begin + j < Img.Pixel.j; j++) {
        memcpy(&TabColor[index_begin + j * Img.Pixel.i],&TileColor[j * TILE_SIZE],min(Img.Pixel.i - rank_i(current_tile,Ci),TILE_SIZE) * sizeof(COLOR));
      }
    }
    // writing in file
    for (j = 0, Color = TabColor; j < size; j++, Color++) {
      Byte = Color->r < 1.0 ? 255.0*Color->r : 255.0;
      putc (Byte, FileImg);
      Byte = Color->g < 1.0 ? 255.0*Color->g : 255.0;
      putc (Byte, FileImg);
      Byte = Color->b < 1.0 ? 255.0*Color->b : 255.0;
      putc (Byte, FileImg);
    }
    EXIT_FILE (FileImg);
    printf("Copied in file\n");
    EXIT_MEM (TabColor);
    printf("Freed TabColor\n");
  }
  EXIT_MEM (TileColor);
  printf("Freed TileColor\n");
  printf("%d finished\n", rank);
  MPI_Finalize();
}

