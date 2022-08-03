/*
 * Kernel.h
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

typedef void interrupt (*pInterrupt)(...);
void interrupt globalTimer(...);
void dispatch();
void tick();




class PCB;
class DummyThread;
class Kernel {
public:
	Kernel();
	virtual ~Kernel();
private:
	static DummyThread* dummy;
	static unsigned timeCounter;
	static int contextChangeOnDemand;
	static void put(PCB* pcb);

	static void init();
	static void restore();
	friend void interrupt gTimer();
	friend void dispatch();
	friend class PCB;
	friend class Thread;
	friend class KernelSem;
	friend class KernelEv;

	friend class TestThread;//xxx obrisati posle
};

#endif /* KERNEL_H_ */
