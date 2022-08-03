/*

 * main.cpp
 *
 *  Created on: 12.09.2020.
 *      Author: OS1
 */

#include <iostream.h>
#include <dos.h>
#include "Kernel.h"
#include "PCB.h"
#include "helper.h"
#include "List.h"
#include "KSem.h"



int userMain(int argc, char* argv[]);



int main(int argc, char* argv[]) {

	PCB* main = new PCB(4084,0, NULL);
	main->started = 1;
	PCB::running = main;

	Kernel* k = new Kernel();
	int ret = 0;

	ret = userMain(argc,argv);

	delete k;
	delete main;
	return ret;
}
