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

#define TILE_SIZE 8

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

 void
 img (const char *FileNameImg)
 {
 	FILE   *FileImg;   
 	COLOR	 *TabColor, *Color, *TileColor;
 	STRING Name;
 	INDEX	 i, j, k,  rank, P;
 	BYTE   Byte;
 	int N = 1898881;
 	MPI_Status status;

 	MPI_Init(NULL, NULL);
 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 	MPI_Comm_size(MPI_COMM_WORLD, &P);

 	MPI_Datatype MPI_COLOR, blocktype, blocktype2;

 	MPI_Type_vector(1, 3, 0, MPI_FLOAT, &MPI_COLOR);
 	MPI_Type_commit(&MPI_COLOR);

 	if (rank == 0) {
 		strcpy (Name, FileNameImg);
 		strcat (Name, ".ppm");
 		INIT_FILE (FileImg, Name, "w");
 		fprintf (FileImg, "P6\n%d %d\n255\n", Img.Pixel.i, Img.Pixel.j);
 	}

 	// number of tiles
 	int Ci = Img.Pixel.i / TILE_SIZE + (Img.Pixel.i % TILE_SIZE?1:0); // number of tiles in dimension i
 	int Cj = Img.Pixel.j / TILE_SIZE + (Img.Pixel.i % TILE_SIZE?1:0);  // number of tiles in dimension j
 	int C = Ci * Cj;
 	N = 1;
 	int q = (C+P-1)/P;
 	int size = Img.Pixel.i * Img.Pixel.j ;
 	if (rank == 0) {
 		// final image buffer
 		INIT_MEM (TabColor, size, COLOR);
 	}
 	// buffer for each tile
 	INIT_MEM (TileColor, TILE_SIZE * TILE_SIZE, COLOR);

 	for (k = rank * q; k <= chinese_remainder_bound(rank, q, C); k++){
    	// pushing tile in queue of current process
 		int current_tile = chinese_remainder_value(k, N, C);;
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
 		MPI_Send(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, 0, current_tile, MPI_COMM_WORLD); 
 	}

 	MPI_Type_vector(TILE_SIZE, TILE_SIZE, size, MPI_COLOR , &blocktype2);
 	MPI_Type_create_resized(blocktype2, 0, sizeof(double), &blocktype);
 	MPI_Type_commit(&blocktype);

  if (rank == 0){ // process 0 gets to know which tiles each process takes care of
  	for (j = 0; j < TILE_SIZE; j++) {
  		memcpy(&TabColor[j * Img.Pixel.i ],&TileColor[j * TILE_SIZE],TILE_SIZE * sizeof(COLOR));
  	}
  	// Receive tiles from other procs
  	for (i = 0; i < C ; i++){
  		MPI_Recv(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  		int j_begin = rank_j(status.MPI_TAG,Cj);
  		int index_begin = rank_i(status.MPI_TAG,Ci) + j_begin * Img.Pixel.i;
  		for (j = 0; j < TILE_SIZE && j_begin + j < Img.Pixel.j; j++) {
  			memcpy(&TabColor[index_begin + j * Img.Pixel.i],&TileColor[j * TILE_SIZE],min(Img.Pixel.i - rank_i(status.MPI_TAG,Ci),TILE_SIZE) * sizeof(COLOR));
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
  	printf("Freed tabcolor\n");
  }
  EXIT_MEM (TileColor);
  MPI_Finalize();
  
}
