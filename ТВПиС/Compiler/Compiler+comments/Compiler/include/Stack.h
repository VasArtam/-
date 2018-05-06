#pragma once
#include "List.h"

class Stack : public List
{
public:
	Stack(int elementCount = LISTSIZE) : List(sizeof(double), elementCount) { };
	~Stack();

	void Push (double);
	double Pop ();
	void Sort(bool dir);
};
