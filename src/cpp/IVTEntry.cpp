/*
 * IVTEntry.cpp
 *
 *  Created on: 25.08.2020.
 *      Author: OS1
 */
#include "helper.h"
#include "IVTEntry.h"
#include <dos.h>
#include "KernelEv.h"

IVTEntry* IVTEntry::table[256];

IVTEntry::IVTEntry(IVTNo ivtNumber, pInterrupt newInterrupt):
	ivtNumber(ivtNumber),
	kernelEvent(0),
	oldInterrupt(0)

{
#ifndef BCC_BLOCK_IGNORE
	asmLock
	oldInterrupt = getvect(ivtNumber);
	setvect(ivtNumber,newInterrupt);
	table[ivtNumber] = this;
	asmUnlock
#endif

}

void IVTEntry::signal() {
	if (kernelEvent != NULL) {
		kernelEvent->signal();
	}
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	asmLock
	setvect(ivtNumber, oldInterrupt);
	asmUnlock
#endif
}

void IVTEntry::signalOld() {
#ifndef BCC_BLOCK_IGNORE
	asmLock
	setvect(0x61, oldInterrupt);
	asm int 61h;
	asmUnlock
#endif
}
void IVTEntry::setKernelEvent(KernelEv* kv) {
	asmLock
	kernelEvent = kv;
	asmUnlock
}
