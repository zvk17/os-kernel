/*
 * Thread.cpp
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#include "Thread.h"
#include "helper.h"
#include "PCB.h"


Thread::Thread (StackSize stackSize, Time timeSlice) {

	asmLock
	myPCB = new PCB(stackSize, timeSlice, this);
	asmUnlock
}


Thread::~Thread() {
	asmLock
	delete myPCB;
	myPCB = NULL;
	asmUnlock
}

void Thread::start() {
	myPCB->start();
}
void Thread::waitToComplete() {
	myPCB->waitToComplete();
}
ID Thread::getId() {
	return myPCB->getId();
}
ID Thread::getRunningId() {
	return PCB::getRunningId();
}
Thread* Thread::getThreadById(ID id) {
	return PCB::getThreadById(id);
}

void Thread::signal(SignalId signal) {
	myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	myPCB->registerHandler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalId id) {
	myPCB->unregisterAllHandlers(id);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	myPCB->swap(id, hand1, hand2);
}

void Thread::blockSignal(SignalId signal) {
	myPCB->blockSignal(signal);
}

void Thread::blockSignalGlobally(SignalId signal) {//static
	PCB::blockSignalGlobally(signal);
}

void Thread::unblockSignal(SignalId signal) {
	myPCB->unblockSignal(signal);
}

void Thread::unblockSignalGlobally(SignalId signal) {//static
	PCB::unblockSignalGlobally(signal);
}
