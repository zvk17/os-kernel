/*

 * helper.cpp

 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */
#include "helper.h"
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include "PCB.h"


//ovaj kod je preuzet sa nekog box-a
// u kome se nalaze i mini testovi

int syncPrintf(const char *format,...) {
    int res;
    va_list args;
	asmLock

	va_start(args,format);
	res = vprintf(format, args);
	va_end(args);

	asmUnlock
	return res;
}
