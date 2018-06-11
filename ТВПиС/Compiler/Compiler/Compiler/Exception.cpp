#include "stdafx.h"
#include "Exception.h"


Exception::Exception(char* message, const int line)
{
	_message = message;
	_line = line;
}


Exception::~Exception()
{

}

char* Exception::GetMessage() const
{
	return _message;
}

int Exception::GetLine() const
{
	return _line;
}
