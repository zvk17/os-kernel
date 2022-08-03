#include "event.h"
#include "KernelEv.h"
#include "helper.h"
Event::Event(IVTNo ivtNo):
	myImpl(0)
{
	asmLock
	myImpl = new KernelEv(ivtNo);
	asmUnlock
}

Event::~Event() {
	asmLock
	delete myImpl;
	myImpl = NULL;
	asmUnlock
}

void Event::wait() {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}
