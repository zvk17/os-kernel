/*
 * KSem.h
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#ifndef KSEM_H_
#define KSEM_H_

#include "PCB.h"
#include "helper.h"
#include "Thread.h"

class List;
class PCB;


class KernelSem {
private:
	class WaitingItem {
	public:
		Time const timeSlice;
		Time timeLeft;
		PCB* pcb;
		WaitingItem(Time ts, PCB* p):
			timeSlice(ts),
			timeLeft(ts),
			pcb(p)
		{}
		~WaitingItem() {
			timeLeft = 0;
			pcb = NULL;
		}
		friend void interrupt globalTimer(...);
	};
public:
	KernelSem(int init);
	virtual ~KernelSem();
	int wait (Time maxTimeToWait);
	int signal(int n=0);
	int val() const;
	int getListSize();//zbog testiranja
private:
	void notify();
	static void notifyAll();

	static List semaphores;
	List* waitingList;
	int value;
	friend void interrupt gTimer();
	friend int main(int argc, char* argv[]);

};

#endif /* KSEM_H_ */
