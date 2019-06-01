/*
 * const.h
 *
 *  Created on: Apr 6, 2019
 *      Author: kimloai
 */

#ifndef CONST_H_
#define CONST_H_

#define NO_READ							0x01
#define READ_ONE_LINE					0x02
#define READ_ONE_LINE_FROM_OTHER_FILE	0x04
#define END_OF_ONE_FILE					0x08
#define END_OF_ALL_FILE					0x10

#define POST_IS_PROCESSED				0x80
#define COMPLETE_IS_PROCESSED			0x100
#define NBARRIER_IS_PROCESSED			0x200
#define PUSH_NBARRIER_TO_DETECTQ		0x400
#define BARRIER_IS_PROCESSED			0x800
#define DETECT_MCE						0x1000

#define TO_DETECT_MCE					-1

#define BUFFER_SIZE             512
#define BASE                    10

#define NON_BLOCK				0

#define DEFAULT                 0
#define FENCE                   1
#define BARRIER                 2
#define GET                     3
#define PUT                     4
#define ACCUMULATE              5
#define POST                    6
#define START                   7
#define COMPLETE                8
#define WAIT                    9
#define LOCK                    10
#define UNLOCK                  11
#define SEND                    12
#define RECV                    13
#define LOAD                    14
#define STORE                   15
#define CREATE                  16

#endif /* CONST_H_ */
