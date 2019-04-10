/*
 * debug.h
 *
 *  Created on: Apr 10, 2019
 *      Author: kimloai
 */


#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#endif /* DEBUG_H_ */
