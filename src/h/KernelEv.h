/*
 * KernelEv.h
 *
 *  Created on: 05.08.2020.
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

typedef unsigned char IVTNo;
class PCB;
class KernelEv {
public:
	KernelEv(IVTNo);
	virtual ~KernelEv();
	void wait();
	void signal();
private:
	int value;
	IVTNo ivtNumber;
	PCB* owner;
};

#endif /* KERNELEV_H_ */
