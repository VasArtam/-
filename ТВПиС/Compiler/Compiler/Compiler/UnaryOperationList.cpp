#include "stdafx.h"
#include "UnaryOperationList.h"
#include <cstring>
#include "defaults.h"
#include "Exceptions.h"

UnaryOperationList::UnaryOperationList(void)
{
	operations = new List(MAX_OPERATION_NAME_LENGHT + 1);
	LoadOperations();
}


UnaryOperationList::~UnaryOperationList(void)
{
	delete operations;
}

UnaryOperationList& UnaryOperationList::Instance()
{
	static UnaryOperationList _instance;
	return _instance;
}

void UnaryOperationList::LoadOperations()
{
	operations->add((void*)"+");		// Унарный плюс 
	operations->add((void*)"-");		// Унарный минус

	operations->add((void*)"!");		// Логическое НЕ и побитовое отрицание

	operations->add((void*)"о++");		// Постфиксный инкремент
	operations->add((void*)"о--");		// Постфиксный декремент
	operations->add((void*)"++о");		// Префиксный инкремент
	operations->add((void*)"--о");		// Префиксный декремент

	operations->add((void*)"input");	// Ввод
	operations->add((void*)"output");	// Вывод

	operations->add((void*)"sin");		// Синус
	operations->add((void*)"cos");		// Косинус
	operations->add((void*)"rand");		// Косинус
}

bool UnaryOperationList::IsOperation(char* name)
{
	return GetOperationIndex(name) < 0 ? false : true;
}

int UnaryOperationList::GetOperationIndex(char* name)
{
	for (int i = 0; i < operations->count(); i++)
		if (strcmp(name, (char*)operations->get_accessor(i)) == 0)
			return i;

	return -1;
}

char* UnaryOperationList::GetOperationName(int index)
{
	if (index < 0 || index >= operations->count())
		return nullptr;

	char* name = (char*)operations->get_accessor(index), *copy = new char[strlen(name) + 1];
	Copy(copy, name, strlen(name));

	return copy;
}
