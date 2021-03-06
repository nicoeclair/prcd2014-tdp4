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
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

#define TILE_SIZE 8
#define NB_THREADS 4

pthread_mutex_t mutex;
sem_t wait_work, ask_work;
MPI_Datatype MPI_COLOR;
INDEX P;
int Ci;
int Cj;
int terminated = 0;
int looking_for_work = 0;
int work_asked = 0;
pthread_mutex_t mutex_time;
long local_time = 0;
int fake = 0;
int vol = 0;

enum tag_e {WORK_ASK, WORK_SEND, TERMINATE, TILE_TAG_INDEX};


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
		//printf("%d - ", e->tile);
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

// functions for chinese remainder
int
chinese_remainder_bound(int rank, int q, int C)
{
	return MIN((rank+1)*q - 1, C - 1);
}

int
chinese_remainder_value(int k, int N, int C)
{
	return (k * N) % C;
}

// calculation of every element for one tile
int tile_fill(struct TileQueue *tiles)
{
	INDEX i,j;
	COLOR *TileColor;
	INIT_MEM(TileColor, TILE_SIZE * TILE_SIZE, COLOR);
	struct timeval t1, t2;
	gettimeofday(&t1, NULL);
	
	while(!terminated){
		pthread_mutex_lock(&mutex);
		if (!isEmpty(tiles)) {
			int current_tile = firstElement(tiles);
			pop(tiles);
			pthread_mutex_unlock(&mutex);

			if (fake) { // fake tasks
				usleep(-current_tile);
			}
			else { // regular tasks
				// assigning first and final index of tile
				int j_begin = rank_j(current_tile, Cj);
				int j_end = MIN(j_begin + TILE_SIZE, Img.Pixel.j);
				int i_begin = rank_i(current_tile, Ci);
				int i_end = MIN(i_begin + TILE_SIZE, Img.Pixel.i);
				for (j = j_begin; j < j_end ; j++) {
					for (i = i_begin ; i < i_end; i++) {
						TileColor [(j-j_begin) * TILE_SIZE + (i-i_begin)] = pixel_basic (i, j);
					}
				}
				// Sending current tile to proc 0
				MPI_Send(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, 0, current_tile + TILE_TAG_INDEX, MPI_COMM_WORLD);
			}
		} else {
			pthread_mutex_unlock(&mutex);
			if (!vol){
				terminated = 1;
				break;
			}
			// if vol de travail, ask for work and wait until we receive it
			sem_post(&ask_work);
			sem_wait(&wait_work);
		}
	}
	
	pthread_mutex_lock(&mutex_time);
	gettimeofday(&t2, NULL);
	local_time = MAX(local_time,(t2.tv_sec - t1.tv_sec)*1000000 + t2.tv_usec - t1.tv_usec);
	printf("time %ld\n",(t2.tv_sec - t1.tv_sec)*1000000 + t2.tv_usec - t1.tv_usec);
	pthread_mutex_unlock(&mutex_time);
	return 0;
}

// function to init tasks & read config file
void init(struct TileQueue* tiles, int rank, int q, int N, int C)
{
	FILE* fd = fopen("config","r");
	int i, k;
	rank--;
	// no config file: regular tasks, calculate image
	if (fd == NULL) {
		fprintf(stderr, "Warning: No config file. Regular tasks & chines remainder.\n");
		for (k = rank * q; k <= chinese_remainder_bound(rank, q, C); k++){
			addTile(tiles,chinese_remainder_value(k, N, C));
		}
		return;
	}
	char buffer[1024];
	int nb_task = 0, sleep_time = 100, repartition = 0;

	fscanf(fd, "%d\n", &fake); // fake tasks ?
	fscanf(fd, "%d\n", &repartition); // chinese remainder ?
	fscanf(fd, "%d\n", &vol); // vol de travail ?

	if (fake == 0) {
  // Chosen method is: regular tasks
		for (k = rank * q; k <= chinese_remainder_bound(rank, q, C); k++){
			addTile(tiles,(repartition==0)?k:chinese_remainder_value(k, N, C));
		}
		fclose(fd);
		return;
	}

	// Chosen method is: fake tasks
	fscanf(fd, "%d\n", &C);
  q = (C+P-1)/P;
  int *sleep_values = malloc(C * sizeof(int));
  k = 0;
  while (fgets(buffer,1024,fd) && k < C){
    sscanf(buffer, "%d %d", &nb_task, &sleep_time);
    // printf("creating %d %d\n",nb_task,sleep_time );
    for (i = 0; i < nb_task && k < C; i++){
      sleep_values[k] = -sleep_time;
      k++;
    }
  }
	for (k = rank * q; k <= chinese_remainder_bound(rank, q, C); k++){
		// printf("Adding %d to queue\n", sleep_values[(repartition == 0)?k:chinese_remainder_value(k, N, C)]);
		addTile(tiles,sleep_values[(repartition == 0)?k:chinese_remainder_value(k, N, C)]);
	}
	free(sleep_values);
	fclose(fd);
}

void
img (const char *FileNameImg)
{
	FILE   *FileImg; 
	COLOR  *TabColor, *Color, *TileColor;
	STRING Name;
	INDEX  i, j, rank;
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

		// Init tasks
		init(&tiles,rank,q,N,C);

		// Init mutex, semaphores & threads
		pthread_mutex_init(&mutex,NULL);
		pthread_mutex_init(&mutex_time,NULL);
		sem_init(&wait_work, 0, 0);
		sem_init(&ask_work, 0, 0);
		pthread_t tid[NB_THREADS];

		for (i = 0; i < NB_THREADS; i++){
			err = pthread_create(&(tid[i]), NULL, (void*)tile_fill, (void*)&tiles);
			if (err != 0)
				printf("\ncan't create thread :[%s]", strerror(err));
		}

		// vol de travail ?
		if (vol){
		// Main thread: Communicator
			while (!terminated)
			{
				int flag = 0, msg;
				MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
				if (flag) {
				// We DID receive a communication so we CAN do a blocking receive
					MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
				// TAG contains the information about what the message is
					switch (status.MPI_TAG){
						case TERMINATE: // No more jobs; threads can finish their jobs and return
						MPI_Isend(&msg, 1, MPI_INT, next_proc, TERMINATE, MPI_COMM_WORLD, &rs);
						terminated = 1;
						break;
						case WORK_ASK: // msg-th process is seeking for job
						pthread_mutex_lock(&mutex);
						if(!isEmpty(&tiles)){
							int tile = firstElement(&tiles);
							pop(&tiles);
							pthread_mutex_unlock(&mutex);
							MPI_Isend(&tile, 1, MPI_INT, msg, WORK_SEND, MPI_COMM_WORLD, &rs);
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
						case WORK_SEND: // Received a job
						pthread_mutex_lock(&mutex);
						addTile(&tiles, msg);
						pthread_mutex_unlock(&mutex);
						sem_post(&wait_work);
						break;
						default: 
						fprintf(stderr, "Err: Unknown message: %d, with tag %d\n", msg,status.MPI_TAG); 
						break;
					}
				}
				if (sem_trywait(&ask_work) == 0){
					MPI_Isend(&rank, 1, MPI_INT, next_proc, WORK_ASK, MPI_COMM_WORLD, &rs);
				}
			}
			for (i = 0; i < NB_THREADS; i++){
				sem_post(&wait_work);
			}
		}

		for (i = 0; i < NB_THREADS; i++){
			pthread_join(tid[i],NULL);
		}

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutex_time);
		sem_destroy(&wait_work);
		sem_destroy(&ask_work);
		fprintf(stderr, "%d %ld\n", rank, local_time);
	}

	// process 0 gathers all the tiles
	if (rank == 0){ 

		// If fake tasks: we don't receive anything and don't write the image
		FILE* fd = fopen("config","r");
		if (fd != NULL){
			int fake;
			fscanf(fd,"%d\n",&fake);
			if (fake){
				EXIT_FILE(FileImg);
				EXIT_MEM(TileColor);
				MPI_Finalize();
				return;
			}
		}

		// final image buffer that will receive the tiles
		INIT_MEM (TabColor, size, COLOR);

		// Receive tiles from other procs
		for (i = 0; i < C ; i++){
			MPI_Recv(TileColor, TILE_SIZE * TILE_SIZE, MPI_COLOR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			int current_tile = status.MPI_TAG - TILE_TAG_INDEX;
			int j_begin = rank_j(current_tile,Cj);
			int index_begin = rank_i(current_tile,Ci) + j_begin * Img.Pixel.i;
			for (j = 0; j < TILE_SIZE && j_begin + j < Img.Pixel.j; j++) {
				memcpy(&TabColor[index_begin + j * Img.Pixel.i],&TileColor[j * TILE_SIZE],MIN(Img.Pixel.i - rank_i(current_tile,Ci),TILE_SIZE) * sizeof(COLOR));
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
	}
	EXIT_MEM (TileColor);
	MPI_Finalize();
}

