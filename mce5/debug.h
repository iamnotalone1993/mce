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

#define PRINT_CLOCK_NICE(fmt, index, clock){\
	static char _return[200];\
	memset(_return, '\0', 200);\
	mpz_t aClock;\
	mpz_init_set(aClock, clock);\
	mpz_t primeFactor;\
	mpz_init_set_ui(primeFactor, 2);\
	int count = 0;\
	while(mpz_cmp_ui(aClock, 1)) {\
		if (mpz_divisible_p(aClock, primeFactor)) {\
			count ++;\
			mpz_div(aClock, aClock, primeFactor);\
			if (mpz_cmp_ui(aClock, 1) == 0) {\
				gmp_sprintf(_return + strlen((char *)_return), "[%Zd] %d ", primeFactor, count);\
			}\
		} else {\
			gmp_sprintf(_return + strlen((char *)_return), "[%Zd] %d ", primeFactor, count);\
			count = 0;\
			mpz_nextprime(primeFactor, primeFactor);\
		}\
	}\
	fprintf (stderr, fmt, index, _return);\
}

#else
#define PRINT_CLOCK(fmt, clock, args...)
#define PRINT_CLOCK_NICE(fmt, index, clock)
#endif

#endif /* DEBUG_H_ */
