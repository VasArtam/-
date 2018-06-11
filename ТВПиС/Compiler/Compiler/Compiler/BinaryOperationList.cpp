#include "stdafx.h"
#include "BinaryOperationList.h"
#include <cstring>
#include "defaults.h"
#include "Exceptions.h"

BinaryOperationList::BinaryOperationList(void)
{
	operations = new List(MAX_OPERATION_NAME_LENGHT);
	LoadOperations();
}


BinaryOperationList::~BinaryOperationList(void)
{
	delete operations;
}

BinaryOperationList& BinaryOperationList::Instance()
{
	static BinaryOperationList _instance;
	return _instance;
}

void BinaryOperationList::LoadOperations()
{
	// Операции присваивания
	operations->add((void*)"=");	// Присваивание

									// Арифметические операции
	operations->add((void*)"+");	// Сложение
	operations->add((void*)"-");	// Вычитание
	operations->add((void*)"*");	// Умножение
	operations->add((void*)"/");	// Деление
	operations->add((void*)"%");	// Остаток от деления

									//Операции сравнения и логические операции
	operations->add((void*)"&&");	// Логическое И
	operations->add((void*)"||");	// Логическое ИЛИ

	operations->add((void*)"==");	// Проверка на равенство
	operations->add((void*)"!=");	// Проверка на неравенство
	operations->add((void*)">");	// Больше
	operations->add((void*)"<");	// Меньше
	operations->add((void*)">=");	// Больше или равно
	operations->add((void*)"<=");	// Меньше или равно

									//Побитовые операции
	operations->add((void*)"&");	// Побитовое И
	operations->add((void*)"|");	// Побитовое ИЛИ
	operations->add((void*)"<<");	// Битовый сдвиг влево
	operations->add((void*)">>");	// Битовый сдвиг право

	operations->add((void*)"[]");	// Квадратные скобки

									// Минимум и максимум
	operations->add((void*)"min");	// Минимум
	operations->add((void*)"max");	// Максимум
}

bool BinaryOperationList::IsOperation(char* name)
{
	return GetOperationIndex(name) < 0 ? false : true;
}

int BinaryOperationList::GetOperationIndex(char* name)
{
	for (int i = 0; i < operations->count(); i++)
		if (strcmp(name, (char*)operations->get_accessor(i)) == 0)
			return i;

	return -1;
}

char* BinaryOperationList::GetOperationName(int index)
{
	if (index < 0 || index >= operations->count())
		return nullptr;

	char* name = (char*)operations->get_accessor(index), *copy = new char[strlen(name) + 1];
	Copy(copy, name, strlen(name));

	return copy;
}
