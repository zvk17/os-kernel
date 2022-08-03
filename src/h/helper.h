/*
 * helper.h
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#ifndef HELPER_H_
#define HELPER_H_

#define asmLock asm pushf; asm cli;
#define asmUnlock asm popf;
#define NULL 0


typedef unsigned int uint16;
typedef unsigned long long uint32;

typedef int int16;
typedef long long int int32;


int syncPrintf(const char *format,...);



#endif /* HELPER_H_ */
