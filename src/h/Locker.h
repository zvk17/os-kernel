/*
 * Locker.h
 *
 *  Created on: 15.09.2020.
 *      Author: OS1
 */

#ifndef LOCKER_H_
#define LOCKER_H_

class Locker {
public:
	static int isLocked();
	static void lock();
	static void unlock();
private:
	Locker();
	virtual ~Locker();
	static int value;
};

#endif /* LOCKER_H_ */
