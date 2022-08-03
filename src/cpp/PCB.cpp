/*
 * PCB.cpp
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#include <dos.h>
#include "PCB.h"
#include "helper.h"
#include "Kernel.h"
#include "List.h"

int PCB::genId = 0;
PCB* volatile PCB::running = NULL;
List PCB::pcbs;
unsigned PCB::globalSignalRegister = ~0;

PCB::PCB(StackSize stackSize2, Time timeSlice2, Thread* thread) {
	asmLock
	id = genId++;
	started = blocked = finished = released = 0;
	timeSlice = timeSlice2;
	stackSize = stackSize2;
	myThread = thread;
	blockedPCBs = new List();
	retVal = 0;
	stack = new unsigned[stackSize];

#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 5] = 0x200;
	stack[stackSize - 6] = FP_SEG(PCB::wrapper);
	stack[stackSize - 7] = FP_OFF(PCB::wrapper);
	sp = FP_OFF(stack + stackSize - 16);
	bp = sp;
	ss = FP_SEG(stack + stackSize - 16);
#endif
	pcbs.pushBack(this);

	/////////////////////////////////////
	///signali
	/////////////////////////////////////
	if (PCB::running != NULL) {
		signalRegister = PCB::running->signalRegister;
	} else {
		signalRegister = ~0;
	}

	for (int j = 0; j < 16; j++) {
		handlerLists[j] = new List();
	}
	unhandledSignals = new List();
	parentPCB = PCB::running;
	if (parentPCB != NULL) {
		for (unsigned i = 0; i < 16; i++) {
			for (List::Iterator it = parentPCB->handlerLists[i]->iterator(); it.hasNext();) {
				SignalHandler* sh = (SignalHandler*)it.next();
				SignalHandler* shCopy = new SignalHandler();
				*shCopy = *sh;
				handlerLists[i]->pushBack(shCopy);
			}
		}
	}
	registerHandler(0, signal0Handler);
	asmUnlock
}

PCB::~PCB() {
	asmLock
	releaseBlockedPCBs();
	delete blockedPCBs;
	delete stack;
	sp = ss = bp = 0;

	for (List::Iterator it = pcbs.iterator(); it.hasNext();) {
		PCB* pcb = (PCB*)it.next();
		if (pcb == this) {
			it.remove();
			break;
		}
	}
	/////////////////////////////
	//signali
	/////////////////////////////
	/////////////////////////////
	for (unsigned j = 0; j < 16; j++) {
		unregisterAllHandlers(j);
		delete handlerLists[j];
		handlerLists[j] = NULL;
	}
	while (!unhandledSignals->isEmpty()) {
		SignalId* s = (SignalId*)unhandledSignals->front();
		unhandledSignals->popFront();
		delete s;
	}
	delete unhandledSignals;
	unhandledSignals = NULL;
	parentPCB = NULL;
	asmUnlock

}

int PCB::getId() {
	return id;
}

int PCB::getRunningId() {
	return running->getId();
}

Thread* PCB::getThreadById(ID id) {
	Thread* t = NULL;
	asmLock
	for (List::Iterator it = pcbs.iterator(); it.hasNext();) {
		PCB* pcb = (PCB*)it.next();
		if (pcb->getId() == id) {
			t = pcb->myThread;
			break;
		}
	}
	asmUnlock
	return t;
}

void PCB::wrapper() {
	PCB* pcb = PCB::running;
	if (pcb->myThread != NULL) {
		pcb->myThread->run();
	}

	asmLock
	pcb->release();
	asmUnlock
	dispatch();
}
void PCB::waitToComplete() {
	asmLock
	if (finished == 0 && PCB::running != this) {
		PCB::running->blocked = 1;
		blockedPCBs->pushBack(PCB::running);
		//syncPrintf("blocking %d on %d\n", PCB::running->getId(), getId());
	}
	asmUnlock
	dispatch();
}
void PCB::start() {
	asmLock
	if (!started) {
		started = 1;
		Kernel::put(this);
	}
	asmUnlock
}
void PCB::printStatus() {
	asmLock
	syncPrintf("ID:%d S:%d B:%d F:%d SP:%d SS:%d BP:%d \n",
				id, started, blocked, finished);
	asmUnlock
}

void PCB::signal(SignalId signal) {
	asmLock
	if (signal >= 0 && signal <= 15 && !finished) {
		SignalId* s = new SignalId();
		*s = signal;
		unhandledSignals->pushBack(s);
	}
	asmUnlock
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	asmLock
	if (signal >= 0 && signal <= 15) {
		SignalHandler* s = new SignalHandler();
		*s = handler;
		handlerLists[signal]->pushBack(s);
	}
	asmUnlock
}

void PCB::unregisterAllHandlers(SignalId id) {
	if (id < 0 || id > 15) {
		return;
	}
	asmLock
	while (!handlerLists[id]->isEmpty()) {
		SignalHandler* s = (SignalHandler*)handlerLists[id]->front();
		handlerLists[id]->popFront();
		delete s;
	}
	asmUnlock
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	if (id < 0 || id > 15) {
		return;
	}
	asmLock
	SignalHandler* s1 = NULL;
	SignalHandler* s2 = NULL;
	for (List::Iterator it = handlerLists[id]->iterator(); it.hasNext();) {
		SignalHandler* s = (SignalHandler*)it.next();
		if (*s == hand1) {
			s1 = s;
		} else if (*s == hand2) {
			s2 = s;
		}
	}
	if (s1 != NULL && s2 != NULL) {
		SignalHandler t = *s1;
		*s1 = *s2;
		*s2 = t;
	}
	asmUnlock
}

void PCB::blockSignal(SignalId signal) {
	asmLock
	signalRegister = PCB::blockIndex(signalRegister, signal);
	asmUnlock
}

void PCB::blockSignalGlobally(SignalId signal) {
	asmLock
	globalSignalRegister = PCB::blockIndex(globalSignalRegister, signal);
	asmUnlock
}

void PCB::unblockSignal(SignalId signal) {
	asmLock
	signalRegister = PCB::allowIndex(signalRegister, signal);
	asmUnlock
}

void PCB::unblockSignalGlobally(SignalId signal) {
	asmLock
	globalSignalRegister = PCB::allowIndex(globalSignalRegister, signal);
	asmUnlock
}

unsigned PCB::allowIndex(unsigned old, unsigned index) {
	if (index >= 0 && index <= 15) {
		unsigned mask = ((unsigned)1) << index;
		old = old | mask;
	}
	return old;
}
unsigned PCB::blockIndex(unsigned old, unsigned index) {
	if (index >= 0 && index <= 15) {
		unsigned mask = ~((unsigned)1 << index) ;
		old = old & mask;
	}
	return old;
}
int PCB::isSignalAllowed(SignalId signal) const {
	asmLock
	int ret = 0;
	unsigned s = globalSignalRegister & signalRegister;
	//syncPrintf("REG:%x S:%d T:%d\n", s,signal, getRunningId());
	if (signal >= 0 && signal <= 15) {
		ret = ((s >> signal) & (unsigned)1);
	}
	asmUnlock
	return ret;
}

void PCB::handleSignals() {
	//syncPrintf("handling signals %d\n", getId());
	for (List::Iterator it = unhandledSignals->iterator(); it.hasNext();) {
		SignalId* s = (SignalId*)it.next();
		if (isSignalAllowed(*s)) {
			//syncPrintf("allowed signal %d\n", *s);
			handleSignal(*s);
			delete s;
			it.remove();
		}
	}
}

void PCB::release() {
	asmLock
	finished = 1;
	releaseBlockedPCBs();

	//signali
	if (parentPCB != NULL) {
		parentPCB->signal(1);
	}
	signal(2);

	asmUnlock
}

void PCB::releaseBlockedPCBs() {
	asmLock
	while (blockedPCBs != NULL && !blockedPCBs->isEmpty()) {
		PCB* pcb = (PCB*)blockedPCBs->front();
		blockedPCBs->popFront();
		pcb->blocked = 0;
		Kernel::put(pcb);
	}
	asmUnlock
}

void PCB::handleSignal(SignalId s) {
	for (List::Iterator it = handlerLists[s]->iterator(); it.hasNext();) {
		SignalHandler* sh = (SignalHandler*)it.next();
		(*sh)();
	}
}
void signal0Handler() {
	PCB::running->released = 1;
}
