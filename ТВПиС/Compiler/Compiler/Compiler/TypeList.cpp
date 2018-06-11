#include "stdafx.h"
#include "TypeList.h"
#include <cstring>
#include "defaults.h"
#include "Exceptions.h"

TypeList::TypeList(void)
{
	types = new List(MAX_TYPE_NAME_LENGHT);
	AddPrimitiveTypes();
}


TypeList::~TypeList(void)
{
	delete types;
}

TypeList& TypeList::Instance()
{
	static TypeList _instance;
	return _instance;
}

void TypeList::AddPrimitiveTypes()
{
	types->add((void*)"int");
	types->add((void*)"float");
	types->add((void*)"double");
	//types->Add((void*)"void");
	//types->Add((void*)"wchar_t");
	types->add((void*)"bool");
	types->add((void*)"char");
}

void TypeList::AddUserType(char* name)
{
	if (name != nullptr && strlen(name) > MAX_TYPE_NAME_LENGHT && !IsType(name)) //@
		throw Exceptions::TypeNameLengthError;

	types->add(name);
}

bool TypeList::IsType(char* name)
{
	//for (int i = 0; i < types->Count(); i++)
	//	if (strcmp(name, (char*)types->GetAccessor(i)) == 0)
	//		return true;

	//return false;

	return GetTypeIndex(name) < 0 ? false : true;
}

int TypeList::GetTypeIndex(char* name)
{
	for (int i = 0; i < types->count(); i++)
		if (strcmp(name, (char*)types->get_accessor(i)) == 0)
			return i;

	return -1;
}

char* TypeList::GetTypeName(int index)
{
	if (index < 0 || index >= types->count())
		return nullptr;

	char* name = (char*)types->get_accessor(index), *copy = new char[strlen(name) + 1];
	Copy(copy, name, strlen(name));

	return copy;
}

