/*
 * IVTEntry.h
 *
 *  Created on: 25.08.2020.
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef void interrupt (*pInterrupt)(...);
typedef unsigned char IVTNo;

class KernelEv;

class IVTEntry {
public:
	IVTEntry(IVTNo ivtNumber, pInterrupt newInterrupt);
	void signalOld();
	void signal();
	virtual ~IVTEntry();
private:
	void setKernelEvent(KernelEv* kv);
	IVTNo ivtNumber;
	pInterrupt oldInterrupt;
	static IVTEntry* table[256];
	friend class KernelEv;
	KernelEv* kernelEvent;
};

#endif /* IVTENTRY_H_ */
