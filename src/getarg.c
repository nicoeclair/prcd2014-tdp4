/****************************************************************************
 ****************************************************************************
 ** File name     : getarg.c                                               **
 ** Creation date : 30 july 1994                                           **
 ** Author        : Gilles Subrenat (email: subrenat@labri.u-bordeaux.fr)  **
 **                                                                        **
 ** Purpose : scan and analyse arguments                                   **
 **                                                                        **
 ** Modification :                                                         **
 **                                                                        **
 ** $Id$
 ****************************************************************************
 ****************************************************************************/

#include <stdio.h> 
#include <string.h>

#include "getarg.h"

char getarg (int    NbrArg,
	     char   **TabArg,
	     char   **NameArg,
	     char   *ListArg)
{
    static	char	*Elt;
    static	char	*Arg = "";
    static	int	Index = 0;
    static	int	Offset = 0;
    static	int	Number = 0;

    if (! *Arg)						     /* New argument */
    {
	if (++Index < NbrArg)
	{
	    Arg = TabArg [Index];
	    Offset = 0;
	}
	else
	{          				            /* Last argument */
	    if ((Elt = strchr(ListArg, ++Number+'0')) && Elt[1] == '!')
	    {
		*NameArg = "Too few argument(s)\n";
		return ('?');
	    }
	    else return (0);
	}
    }

    if (*Arg == '-')        				/* Optional argument */
    {
	if ((Elt = strchr (ListArg, Arg [++Offset])))    /* Arg does exist */
	{
	    if (Elt [1] == '.')      		       /* Arg with parameter */
	    {
		if (Arg [Offset+1])
		{
		    *NameArg = Arg + Offset + 1;
		}
		else if (++Index < NbrArg)
		{
		    *NameArg = TabArg [Index];
		}
		else
		{
		    *NameArg = "Argument needs parameter\n";
		    return ('?');
		}
		Arg = "";
		Offset = 0;
		return (*Elt);
	    }
	    else       				    /* Arg without parameter */
	    {
		if (!Arg [Offset+1])
		    Arg = "";
		*NameArg = "";
		return (*Elt);
	    }
	}
	else          				       /* Arg does not exist */
	{
	    *NameArg = "Unknown argument\n";
	    return ('?');
	}
    }
    else          				      /* Compulsory argument */
    {
	if ((Elt = strchr (ListArg, ++Number + '0')))
	{
	    *NameArg = TabArg [Index];
	    Arg  = "";
	    return (Number + '0');
	}
	else
	{
	    *NameArg = "Too many argument(s)\n";
	    return ('?');
	}
    }
}
