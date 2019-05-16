/*
 * debug.h
 *
 *  Created on: Apr 10, 2019
 *      Author: kimloai
 */


#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG
#define DEBUG2

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#ifdef DEBUG2
#define PRINT_CLOCK(fmt, index, clock){\
	char *string = malloc(sizeof(char) * mpz_sizeinbase(clock, 10));\
	mpz_get_str(string, 10, clock);\
	fprintf (stderr, fmt, index, string);\
	free(string);\
}
#else
#define PRINT_CLOCK(fmt, clock, args...)
#endif

#endif /* DEBUG_H_ */
