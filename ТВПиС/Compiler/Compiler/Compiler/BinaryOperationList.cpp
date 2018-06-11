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
	// �������� ������������
	operations->add((void*)"=");	// ������������

									// �������������� ��������
	operations->add((void*)"+");	// ��������
	operations->add((void*)"-");	// ���������
	operations->add((void*)"*");	// ���������
	operations->add((void*)"/");	// �������
	operations->add((void*)"%");	// ������� �� �������

									//�������� ��������� � ���������� ��������
	operations->add((void*)"&&");	// ���������� �
	operations->add((void*)"||");	// ���������� ���

	operations->add((void*)"==");	// �������� �� ���������
	operations->add((void*)"!=");	// �������� �� �����������
	operations->add((void*)">");	// ������
	operations->add((void*)"<");	// ������
	operations->add((void*)">=");	// ������ ��� �����
	operations->add((void*)"<=");	// ������ ��� �����

									//��������� ��������
	operations->add((void*)"&");	// ��������� �
	operations->add((void*)"|");	// ��������� ���
	operations->add((void*)"<<");	// ������� ����� �����
	operations->add((void*)">>");	// ������� ����� �����

	operations->add((void*)"[]");	// ���������� ������

									// ������� � ��������
	operations->add((void*)"min");	// �������
	operations->add((void*)"max");	// ��������
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
