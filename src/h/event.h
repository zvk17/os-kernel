/*
 * event.h
 *
 *  Created on: 05.08.2020.
 *      Author: OS1
 */

#ifndef _event_h_
#define _event_h_

#include "IVTEntry.h"

#define PREPAREENTRY(entryNumber, callOld)\
		void interrupt inter##entryNumber(...);\
		IVTEntry ivtEntry##entryNumber(entryNumber, inter##entryNumber);\
		void interrupt inter##entryNumber(...) {\
			ivtEntry##entryNumber.signal();\
			if (callOld == 1) {\
				ivtEntry##entryNumber.signalOld();\
			}\
		}\


typedef unsigned char IVTNo;
class KernelEv;




class Event {
public:
	Event(IVTNo ivtNo);
	~Event ();
	void wait();
protected:
	friend class KernelEv;
	void signal(); // can call KernelEv
private:
	KernelEv* myImpl;
};




#endif /* _event_h_ */
