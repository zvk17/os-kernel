/*
 * List.h
 *
 *  Created on: 13.09.2020.
 *      Author: OS1
 */

#ifndef LIST_H_
#define LIST_H_
#include "helper.h"

class List {
private:
	class Node {
	public:
		void* data;
		Node* next;
		Node* prev;
		Node(void* d) {
			data = d;
			next = prev = NULL;
		}
		~Node() {
			data = NULL;
			next = prev = NULL;
		}

	};

public:
	class Iterator {
	public:
		int hasNext();
		void* next();
		void* remove();
		~Iterator(){node = NULL;}
	private:
		Node* node;
		List& owner;
		Iterator(List& list, Node* n):owner(list), node(n) {}
		friend class List;
	};
	friend class Iterator;
	List();
	virtual ~List();
	int size();
	int isEmpty();
	void pushBack(void*d);
	void pushFront(void*d);
	void popBack();
	void popFront();
	void* front();
	void* back();
	Iterator iterator();
private:
	void removeBetween(Node* current);
	void insertBetween(Node* prev, Node* next, Node* current);
	Node* head;
	Node* tail;
	int sz;

};

#endif /* LIST_H_ */
