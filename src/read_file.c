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

#include "type.h"
#include "const.h"
#include "macro.h"

#include "ett.h"
#include "scn.h"
#include "bnd.h"
#include "img.h"
#include "cmr.h"

#include "read_file.h"


#ifndef ZCAT_COMMAND
#define ZCAT_COMMAND "zcat"
#endif

/*
 * MainEtt is built only once, so it must not be built
 * in a recursive call of read_scn
 */
static int TopCall = 0;

static int
file_command (FILE *File)
{
  int   Command;

  while ((Command = getc (File)) != EOF)
    switch (Command) {
      case ' ' : case '\t' : break;
      case '#' : while (getc (File) != '\n');
      case '\n': return ('#');
      default  : return (Command);
    }
  return (EOF);   
}


void
read_scn (char *FileName)
{
  int     Command;
  FILE    *File;
  BOOL    IsGzipped = FALSE;
  STRING  NewFileName;
  LST     *Lst;
  static LST *LstEtt = NULL;
  INDEX   Index;

  TopCall ++;  /* one more recursive level */

  if ((File = fopen (FileName, "r")) == NULL) {
    char  *UnixCommand = NULL;
    int   UnixCommandLength = strlen (FileName) + strlen (ZCAT_COMMAND) + 5;

    IsGzipped = TRUE;
    INIT_MEM (UnixCommand, UnixCommandLength, char);
    sprintf (UnixCommand, "%s %s.gz", ZCAT_COMMAND, FileName);
    File = popen (UnixCommand, "r");
    EXIT_MEM (UnixCommand);
  }

  EXIT_IF (File == NULL, "fichier scene non trouve");

  while ((Command = file_command (File)) != EOF)
    switch (Command) {
      case '#': break;
      case 'E':
	INIT_MEM (Lst, 1, LST);
	Lst->Data = file_ett (File);
	Lst->Next = LstEtt;
	LstEtt = Lst;
	NbrEtt ++;
	break;
      case 'S': file_scn (File); break;
      case 'B': file_bnd (File); break;
      case 'I': file_img (File); break;
      case 'C': file_cmr (File); break;
      case '$': fscanf (File, "%s", NewFileName);
	        read_scn (NewFileName);
		break;
      default : EXIT_NOIF ("Syntax error in description file");
    }

  if (IsGzipped)
    pclose (File);
  else
    fclose (File);

  if (TopCall == 1) {
    INIT_MEM (MainEtt, NbrEtt, ETT*);
    for (Index = 0; (Lst = LstEtt) != NULL; Index ++) {
      MainEtt[Index] = Lst->Data;
      LstEtt = Lst->Next;
      EXIT_MEM (Lst);
    }
  }

  TopCall --;
}

