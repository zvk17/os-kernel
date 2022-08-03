/*
 * List.cpp
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#include "List.h"
#include "helper.h"

List::List() {

	asmLock
	head = new Node(NULL);
	tail = new Node(NULL);
	head->next = tail;
	tail->prev = head;
	sz = 0;
	asmUnlock

}

List::~List() {
	asmLock
	while (!isEmpty()) {
		popFront();
	}
	delete head;
	delete tail;
	head = tail = NULL;
	asmUnlock
}

int List::size() {
	return sz;
}
int List::isEmpty() {
	return size() == 0;
}

void List::insertBetween(Node* prev, Node* next, Node* current) {
	asmLock
	current->prev = prev;
	current->next = next;
	prev->next = current;
	next->prev = current;
	asmUnlock
}
void List::pushBack(void*d) {
	asmLock
	sz++;
	Node* n = new Node(d);
	insertBetween(tail->prev, tail, n);
	asmUnlock
}
void List::pushFront(void*d) {
	asmLock
	sz++;
	Node* n = new Node(d);
	insertBetween(head, head->next, n);
	asmUnlock
}
void List::popBack() {
	asmLock
	if (!isEmpty()) {
		removeBetween(tail->prev);
	}
	asmUnlock
}
void List::popFront() {
	asmLock
	if (!isEmpty()) {
		removeBetween(head->next);
	}
	asmUnlock
}
void List::removeBetween(Node* current) {
	asmLock
	sz--;
	Node* prev = current->prev;
	Node* next = current->next;
	prev->next = next;
	next->prev = prev;
	delete current;
	asmUnlock
}
void* List::front() {
	asmLock
	void* ret = NULL;
	if (!isEmpty()) {
		ret = head->next->data;
	}
	asmUnlock
	return ret;
}
void* List::back() {
	asmLock
	void* ret = NULL;
	if (!isEmpty()) {
		ret = tail->prev->data;
	}
	asmUnlock
	return ret;
}
List::Iterator List::iterator() {
	return Iterator(*this, head);
}

int List::Iterator::hasNext() {
	return (node != NULL) && (node->next != owner.tail);
}
void* List::Iterator::next() {
	void* ret = NULL;
	asmLock
	if (hasNext()) {
		node = node->next;
		ret = node->data;
	}
	asmUnlock
	return ret;
}
void* List::Iterator::remove() {
	asmLock
	if ((node != owner.head) && !owner.isEmpty()) {

		Node* prev = node->prev;
		owner.removeBetween(node);
		node = prev;

	}
	asmUnlock
}
