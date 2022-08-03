/*
 * Kernel.cpp
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#include "Locker.h"
#include "Kernel.h"
#include "KSem.h"
#include "DThread.h"
#include "helper.h"
#include "PCB.h"
#include "SCHEDULE.h"
#include <dos.h>
#include "List.h"


unsigned volatile tsp = 0;
unsigned volatile tss = 0;
unsigned volatile tbp = 0;

void interrupt gTimer();

unsigned Kernel::timeCounter = 0;

DummyThread* Kernel::dummy = NULL;
int Kernel::contextChangeOnDemand = 0;

unsigned oldTimerOFF;
unsigned oldTimerSEG;

Kernel::Kernel() {
	asmLock
	dummy = new DummyThread();
	((Thread*)dummy)->myPCB->finished = 1;
	dummy->start();
	Kernel::init();
	asmUnlock
}

Kernel::~Kernel() {
	asmLock
	Kernel::restore();
	delete dummy;
	dummy = 0;
	asmUnlock
}

void Kernel::init() {
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg gTimer
		mov word ptr es:0020h, offset gTimer
		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

void Kernel::restore() {
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}
void dispatch() {
	asmLock
	Kernel::contextChangeOnDemand = 1;
	gTimer();
	asmUnlock
}

void interrupt gTimer() {
	if (!Kernel::contextChangeOnDemand) {
		asm int 60h;
		KernelSem::notifyAll();
		tick();
		if (Kernel::timeCounter > 0) {
			Kernel::timeCounter--;
		}
	}

	if (
		(Kernel::contextChangeOnDemand || PCB::running->released ||
			(Kernel::timeCounter == 0 && PCB::running->timeSlice != 0)
		)
			&& !Locker::isLocked()
	){
		asm {
			mov tbp, bp
			mov tsp, sp
			mov tss, ss
		}
		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		if (PCB::running->released) {
			PCB::running->release();
		}
		Kernel::put(PCB::running);
		PCB::running = Scheduler::get();

		if (PCB::running == NULL) {
			PCB::running = ((Thread*)Kernel::dummy)->myPCB;
		} else if (PCB::running->released) {
			PCB::running->release();
			PCB::running = ((Thread*)Kernel::dummy)->myPCB;
		}

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;
		Kernel::timeCounter = PCB::running->timeSlice;


		asm {
			mov bp, tbp
			mov sp, tsp
			mov ss, tss
		}

	}
	Kernel::contextChangeOnDemand = 0;
	if (!Locker::isLocked()) {
		Locker::lock();
		asm pushf;
		asm sti;

		PCB::running->handleSignals();

		asm popf;
		Locker::unlock();
	}




}
void Kernel::put(PCB* pcb) {
	if (pcb == NULL) {
		return;
	}
	asmLock
	if (!pcb->blocked && pcb->started && !pcb->finished) {
		Scheduler::put(pcb);
	}
	asmUnlock
}
