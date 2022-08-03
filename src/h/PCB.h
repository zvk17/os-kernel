/*
 * PCB.h
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_
#include "Thread.h"
class List;
class PCB {
public:
	PCB(StackSize stackSize, Time timeSlice2, Thread* thread);
	virtual ~PCB();
	int blocked;
	int started;
	int finished;
	int retVal;
	int released;
	unsigned sp;
	unsigned ss;
	unsigned bp;

	unsigned* stack;

	Time timeSlice;
	StackSize stackSize;
	void start();
	int getId();
	static int getRunningId();
	static Thread* getThreadById(ID id);
	void printStatus();
private:
	List* blockedPCBs;
	int id;
	Thread* myThread;
	static int genId;


	void releaseBlockedPCBs();
	void waitToComplete();
	static PCB* volatile  running;
	static void wrapper();

	friend class KernelEv;
	friend class KernelSem;
	friend class Thread;
	friend class Kernel;
	friend void printPSWIBit();
	friend void interrupt gTimer();
	friend int main(int argc, char* argv[]);
	static List pcbs;
	// signali
public:

	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

private:
	static unsigned globalSignalRegister;
	unsigned signalRegister;
	PCB* parentPCB;

	static unsigned allowIndex(unsigned old, unsigned index);
	static unsigned blockIndex(unsigned old, unsigned index);
	void release();
	int isSignalAllowed(SignalId signal) const;
	void handleSignals();
	void handleSignal(SignalId s);
	friend void signal0Handler();
	List* handlerLists[16];
	List* unhandledSignals;


};

#endif /* PCB_H_ */
