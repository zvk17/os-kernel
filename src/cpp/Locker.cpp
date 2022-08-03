/*
 * Locker.cpp
 *
 *  Created on: 15.09.2020.
 *      Author: OS1
 */

#include "Locker.h"
#include "helper.h"

Locker::Locker() {}
Locker::~Locker() {}

int Locker::value = 0;
int Locker::isLocked() {
	asmLock
	int ret = value > 0;
	asmUnlock
	return ret;
}
void Locker::lock() {
	asmLock
	value++;
	asmUnlock
}
void Locker::unlock() {
	asmLock
	if (value > 0)
		value--;
	asmUnlock
}
