#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "definition.h"
#include "modeleur.h"


#define  SKIP_LINE(fd)     while (getc(fd) != '\n')




int main (int argc, char *argv[])
{
    char    fich_in[30];
    char    fich_out[30];
    char    Extension[30];
    FILE *  fd_in;
    FILE *  fd_out;
    char    ligne[210];
    int     i;

    int     NumOpt = 0;
    int     NumGeo = 0;
    int     NumEtt = 0;
    int     NbrPnt;
    POINT   Pnt;
    char    c;
    float   coeff, radius, r, g, b;

    /*----------------------------*
     | test du nombre d'arguments |
     *----------------------------*/
    if (argc != 4)
    {
        printf ("usage : %s fichIn fichOut Extension\n",argv[0]);
	return EXIT_FAILURE;
    }


    /*----------------------------------*
     | lecture et analyse des arguments |
     *----------------------------------*/
    strcpy (fich_in,  argv[1]);
    strcpy (fich_out, argv[2]);
    strcpy (Extension, argv[3]);


    /*----------------------*
     | ouverture du fichier |
     *----------------------*/
    fd_in = fopen (fich_in, "r+");
    if (fd_in == NULL)
    {
        printf ("impossible d'ouvrir %s\n", fich_in);
	return EXIT_FAILURE;
    }

    fd_out = fopen (fich_out, "w+");
    if (fd_in == NULL)
    {
        printf ("impossible d'ouvrir %s\n", fich_out);
	return EXIT_FAILURE;
    }


    while (fgets (ligne, 210, fd_in) != NULL)
    {
        switch (ligne[0])
	{
	    case 'v' :
	      fgets (ligne, 210, fd_in);
	      fgets (ligne, 210, fd_in);
	      fgets (ligne, 210, fd_in);
	      fgets (ligne, 210, fd_in);
	      fgets (ligne, 210, fd_in);
	      fgets (ligne, 210, fd_in);
	      break;


	    case 'f' :
	      sscanf (ligne, "%c %f %f %f %f", &c, &r, &g, &b, &coeff);
	      NumOpt ++;
	      break;
	      

	    case 'p' :
	      fprintf (fd_out, "E NOLIGHT  %g %g %g   %g\n", r, g, b, coeff);
	      sscanf (ligne, "%c %d", &c, &NbrPnt);
	      NumGeo ++;
	      fprintf (fd_out, " NAPPE %d 1\n", NbrPnt);
	      for (i=0; i<NbrPnt; i++)
	      {
		  fgets (ligne, 210, fd_in);
		  fprintf (fd_out ,"%s", ligne);
	      }
	      if (NbrPnt == 3)
		  fprintf (fd_out ,"(0 1 2 0)\n");
	      else
		  fprintf (fd_out ,"(0 1 2 3)\n");
	      NumEtt ++;
	      fprintf (fd_out ,"\n");
	      break;

	    case 's' :
	      fprintf (fd_out, "E NOLIGHT  %g %g %g   %g\n", r, g, b, coeff);
	      sscanf (ligne, "%c %f %f %f %f", &c, &(Pnt.x), &(Pnt.y), &(Pnt.z), &radius);
	      NumGeo ++;
	      fprintf (fd_out ," SPHERE  %g %g %g  %g\n", Pnt.x, Pnt.y, Pnt.z, radius);
	      NumEtt ++;
	      fprintf (fd_out ,"\n");
	      break;
	}
    }
    fprintf (fd_out, "\n");

    fclose (fd_out);
    fclose (fd_in);

    return EXIT_SUCCESS;
}
