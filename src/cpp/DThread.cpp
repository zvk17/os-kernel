/*
 * DThread.cpp
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#include "DThread.h"
#include "helper.h"
DummyThread::DummyThread()
{
}

DummyThread::~DummyThread() {

}
void DummyThread::run() {
	while (1) {}
}
