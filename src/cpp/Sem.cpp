/*
 * Semaphore.cpp
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#include "semaphor.h"
#include "helper.h"
#include "KSem.h"

Semaphore::Semaphore(int init) {
	asmLock
	myImpl = new KernelSem(init);
	asmUnlock
}

Semaphore::~Semaphore() {
	asmLock
	delete myImpl;
	asmUnlock
}
int Semaphore::wait (Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}
int Semaphore::signal(int n) {
	return myImpl->signal(n);
}
int Semaphore::val () const {
	return myImpl->val();
}
