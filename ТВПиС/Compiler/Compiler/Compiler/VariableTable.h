#pragma once
#include "Hash.h"
#include "TValueKeeper.h"

// Хеш-таблица для хранения значений TValue
// и имен констант и переменных их хранящих.

class VariableTable : public Hash
{
public:
	VariableTable();

	~VariableTable();

	int key1(char*) override;

	TValueKeeper* Find(char*);

	void Push(TValueKeeper*);
	void Push(char* name, double value, int type, int size = 0);
};
