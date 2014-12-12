/****************************************************************************
 ****************************************************************************
 ** File name     : getarg.h                                               **
 ** Creation date : 30 july 1994                                           **
 ** Author        : Gilles Subrenat (email: subrenat@labri.u-bordeaux.fr)  **
 **                                                                        **
 ** Purpose : header file of getarg.c                                      **
 **                                                                        **
 ** Modification :                                                         **
 **                                                                        **
 ** $Id$
 ****************************************************************************
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 |                           How to use it                                  |
 *--------------------------------------------------------------------------*
 | For exemple if your program takes the following optional arguments :     |
 |       -c "image title" : to give a title to the image                    |
 |       -i               : to be on inverse video                          |
 |       -t tmp_dir       : to indicate a temporary directory               |
 |       -v               : to be on verbose mode                           |
 |       -z zoom          : to zoom the final image                         |
 |                                                                          |
 | and the following compulsory arguments :                                 |
 |       first one  : the name of the file describing the scene to render   |
 |       second one : the name of the file describing the observer          |
 |       third one  : the name of the file which will contain the image     |
 |                                                                          |
 | As you see there are five optional argument with three needing a         |
 | parameter.                                                               |
 | There are also three compulsory arguments.                               |
 | In the command line, these arguments may be put at any order, the only   |
 | order to respect is to put the first compulsory argument before the      |
 | the second one, and the second one before the thirs one. For example,    |
 | the following command lines are valid :                                  |
 |                                                                          |
 | a.out -v -c "salle bleue" descript.txt viewer.txt image.ppm              |
 | a.out -c "salle bleue" descript.txt viewer.txt -v image.ppm              |
 | a.out descript.txt -c "salle bleue" viewer.txt image.ppm -v              |
 | a.out descript.txt viewer.txt image.ppm -v -c "salle bleue"              |
 |                                                                          |
 | a.out -iv -t /tmp descript.txt viewer.txt image.ppm                      |
 | a.out -i -v descript.txt viewer.txt image.ppm -t /tmp                    |
 | a.out descript.txt -ivt /tmp viewer.txt image.ppm                        |
 |                                                                          |
 |                                                                          |
 | your main program will look like this :                                  |
 |                                                                          |
 | #include "string.h"                                                      |
 | #include "getarg.h"                                                      |
 |                                                                          |
 | main (int NbrArg, char **TabArg)                                         |
 | {                                                                        |
 |     char   Arg,                                                          |
 |           *NameArg;                                                      |
 |                                                                          |
 |     if (NbrArg < 4)                                                      |
 |         usage ();                                                        |
 |                                                                          |
 |     while (Arg = getarg (NbrArg, TabArg, &NameArg, "c.it.vz.1!2!3!"))    |
 |     {                                                                    |
 |         switch (Arg)                                                     |
 |         {                                                                |
 |           case '1' : strcpy (NameScn, NameArg); break;                   |
 |           case '2' : strcpy (NameVwr, NameArg); break;                   |
 |           case '3' : strcpy (NameImg, NameArg); break;                   |
 |           case 'c' : strcpy (Title, NameArg);   break;                   |
 |           case 'i' : InverseFlag = TRUE;        break;                   |
 |           case 't' : strcpy (TmpDir, NameArg);  break;                   |
 |           case 'v' : VerboseFlag = TRUE;        break;                   |
 |           case 'z' : Zoom = atoi(NameArg);      break;                   |
 |           case '?' : fprintf (stderr, "%s : Error in command line : %s\n",
 |                               TabArg[0], NameArg);                       |
 |                      usage ();                                           |
 |                      break;                                              |
 |         }                                                                |
 |     }                                                                    |
 | }                                                                        |
 |                                                                          |
 *--------------------------------------------------------------------------*/

#ifndef _GETARG_H_
#define _GETARG_H_


extern  char getarg (int,         /* NbrArg  : usually argc from main */
                     char **,     /* TabArg  : usually argv from main */
                     char **,     /* NameArg : an argument */
                     char *      /* ListArg : which options are available */
                    );


#endif
