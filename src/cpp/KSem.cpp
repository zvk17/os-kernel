/*
 * KSem.cpp
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#include "KSem.h"
#include "helper.h"
#include "List.h"
#include "Kernel.h"



List KernelSem::semaphores;

KernelSem::KernelSem(int init) {
	asmLock
	waitingList = new List();
	value = init;
	semaphores.pushBack(this);
	asmUnlock
}

KernelSem::~KernelSem() {
	asmLock
	delete waitingList;
	waitingList = NULL;
	value = 0;
	for (List::Iterator it = semaphores.iterator(); it.hasNext();) {
		KernelSem* ksem = (KernelSem*)it.next();
		if (ksem == this) {
			it.remove();
			break;
		}
	}
	asmUnlock
}
int KernelSem::wait (Time maxTimeToWait) {
	asmLock
	value--;

	if (value >= 0) {
		asmUnlock
		return 1;
	}

	PCB::running->blocked = 1;
	PCB::running->retVal = 0;
	WaitingItem* wi = new WaitingItem(maxTimeToWait, PCB::running);
	waitingList->pushBack(wi);
	asmUnlock
	dispatch();
	asmLock
	int ret = !PCB::running->retVal;
	asmUnlock

	return ret;
}
int KernelSem::signal(int n) {
	if (n < 0) {
		return n;
	}
	asmLock
	int x = n;
	if (n == 0) {
		x = 1;
	}
	value += x;

	int i = 0;
	while (i < x && !waitingList->isEmpty()) {
		WaitingItem* wi = (WaitingItem*)waitingList->front();
		waitingList->popFront();
		if (waitingList != NULL) {
			PCB* pcb = wi->pcb;
			delete wi;
			pcb->blocked = 0;
			Kernel::put(pcb);
			i++;
		}

	}
	if (n == 0)
		i = 0;
	asmUnlock
	return i;
}
int KernelSem::val() const {
	asmLock
	int r = value;
	asmUnlock
	return r;
}

int KernelSem::getListSize() {
	asmLock
	int sz = waitingList->size();
	asmUnlock
	return sz;
}
void KernelSem::notifyAll() {
	if (semaphores.isEmpty()) {
		return;
	}
	asmLock
	for (List::Iterator it = semaphores.iterator(); it.hasNext();) {
		KernelSem* ksem = (KernelSem*)it.next();
		if (ksem != NULL) {
			ksem->notify();
		}
	}
	asmUnlock
}

void KernelSem::notify() {
	if (waitingList->isEmpty()) {
		return;
	}
	asmLock
	for (List::Iterator it = waitingList->iterator(); it.hasNext();) {
		WaitingItem* wi = (WaitingItem*)it.next();

		if (wi != NULL && wi->timeSlice != 0) {
			wi->timeLeft--;
			if (wi->timeLeft <= 0) {
				value++;
				PCB* pcb = wi->pcb;
				pcb->blocked = 0;
				pcb->retVal = 1;
				Kernel::put(pcb);
				delete wi;
				it.remove();
			}
		}
	}
	asmUnlock
}


