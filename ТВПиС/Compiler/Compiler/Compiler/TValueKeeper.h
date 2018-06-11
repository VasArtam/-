#pragma once
#include "TValue.h"

class TValueKeeper
{
	char* _name;
	TValue* _value;
	bool _const;

public:
	TValueKeeper(char* name, TValue* value, bool _const = false);

	~TValueKeeper();

	char* GetName() const;

	TValue* GetValue() const;
	void SetValue(TValue*);

	bool IsConst() const;
	bool IsReference() const;
};
