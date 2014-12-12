/* Copyright Achille (book) */
/* modified with EXIT_NOIF macro */

#ifndef EXIT_IF_H
#define EXIT_IF_H

#include <stdio.h>

#define EXIT_IF(expr, mess) \
        ((expr) \
	 ? (fprintf(stderr, ">EXIT:%s:%d: %s\n", \
		    __FILE__, __LINE__, mess), exit(1)) \
	 : 0)

#define EXIT_NOIF(mess) \
	 (fprintf(stderr, ">EXIT:%s:%d: %s\n", \
		    __FILE__, __LINE__, mess), exit(1))

#endif /* EXIT_IF_H */
