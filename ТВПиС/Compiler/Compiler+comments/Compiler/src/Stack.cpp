#include "stdafx.h"
#include "Stack.h"

void Stack::Push(double x) //добавление элемента в стек
{
	Add(&x);
}

double Stack::Pop() {
	double res;
	TakeLast(&res);
	return res;
}

Stack::~Stack()
{
	List::~List();
}


