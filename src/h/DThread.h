/*
 * DThread.h
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#ifndef DTHREAD_H_
#define DTHREAD_H_

#include "Thread.h"
class DummyThread: public Thread {
public:
	DummyThread();
	virtual void run();
	virtual ~DummyThread();
};

#endif /* DTHREAD_H_ */
