#include "stdafx.h"
#include "VariableTable.h"

VariableTable::VariableTable() : Hash(9, 0, 0, 0, 0)
{
}

VariableTable::~VariableTable()
{
	Hash::~Hash();
}

int VariableTable::key1(char* keyWord)
{
	return keyWord ? static_cast<int>(*keyWord) % 10 : 0;
}

TValueKeeper* VariableTable::Find(char* name)
{
	List* list = find_list(name);

	if (list != nullptr)
	{
		TValueKeeper* keeper = nullptr;

		for (int i = 0; i < list->count(); i++)
		{
			if (strcmp((keeper = static_cast<TValueKeeper*>(list->get_accessor(i)))->GetName(), name) == 0)
			{
				return keeper;
			}
		}
	}

	return nullptr;
}

void VariableTable::Push(TValueKeeper* keeper)
{
	if (keeper)
	{
		List* list = table[combine_keys(keeper->GetName())];

		if (list == nullptr)
		{
			list = new List(sizeof(TValueKeeper));
		}

		list->add(keeper);
	}
}

void VariableTable::Push(char* name, double value, int type, int size)
{
	Push(new TValueKeeper(name, new TValue(value, type, size)));
}
