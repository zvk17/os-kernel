/*
 * KernelEv.cpp
 *
 *  Created on: 05.08.2020.
 *      Author: OS1
 */
#include "helper.h"
#include "KernelEv.h"
#include "Kernel.h"
#include "IVTEntry.h"
#include "PCB.h"

KernelEv::KernelEv(IVTNo ivtNumber):value(0), owner(0), ivtNumber(ivtNumber) {
	asmLock
	IVTEntry::table[ivtNumber]->setKernelEvent(this);
	owner = PCB::running;
	asmUnlock
}

KernelEv::~KernelEv() {
	asmLock
	IVTEntry::table[ivtNumber]->setKernelEvent(NULL);
	asmUnlock
}

void KernelEv::wait() {
	asmLock
	if (PCB::running == owner) {
		if (value == 1) {
			value = 0;

		} else if (value <= 0) {
			value--;

			owner->blocked = 1;
			asmUnlock
			dispatch();
			return;
		}
	}


	asmUnlock
}

void KernelEv::signal() {
	asmLock

	if (value < 0) {
		value = 0;
		owner->blocked = 0;
		Kernel::put(owner);
	} else {
		value = 1;
	}
	asmUnlock
	dispatch();
	// PIŠE u zadatku da treba uvek vrsiti preuzimanje

}
