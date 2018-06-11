#include "stdafx.h"
#include "Automat.h"
#include "Parser.h"
#include "Exceptions.h"
#include <iostream>

Automat::Token Automat::StrToToken(char* s)
{
	if (!strcmp(s, "SLZSymbolComment"))
		return Token::SingleLineComment;

	if (!strcmp(s, "MLZSymbolComment"))
		return Token::MultilineComment;

	if (!strcmp(s, "zeroSymbolString"))
		return Token::String;

	if (!strcmp(s, "isreservedWord"))
		return Token::ReservedWord;

	if (!strcmp(s, "zeroSymbolDigit"))
		return Token::Digit;

	if (!strcmp(s, "zeroSymbolChar"))
		return Token::Char;

	if (!strcmp(s, "isbracket"))
		return Token::Bracket;

	if (!strcmp(s, "isStar"))
		return Token::Star;

	if (!strcmp(s, "isCompOper"))
		return Token::CompOper;

	if (!strcmp(s, "isLogicOper"))
		return Token::LogicOper;

	if (!strcmp(s, "isBiteOp"))
		return Token::BiteOp;

	if (!strcmp(s, "isAssignment"))
		return Token::Assignment;

	if (!strcmp(s, "isSeparator"))
		return Token::Separator;

	if (!strcmp(s, "isUserType"))
		return Token::UserType;

	if (!strcmp(s, "isSpace"))
		return Token::Space;

	if (!strcmp(s, "isNewLine"))
		return Token::NewLine;

	if (!strcmp(s, "isReservedType"))
		return Token::ReservedType;

	if (!strcmp(s, "isCondition"))
		return Token::Condition;

	if (!strcmp(s, "isSysFunc"))
		return Token::SysFunction;

	if (!strcmp(s, "isArOp"))
		return Token::ApOp;

	if (!strcmp(s, "isIncOrDec"))
		return Token::IncOrDec;

	return {};
}

bool Automat::IsLetter(char c)
{
	if (c >= static_cast<char>('a') && c <= static_cast<char>('z')) return true;
	if (c >= static_cast<char>('A') && c <= static_cast<char>('Z')) return true;

	return false;
}

Automat::Automat(char* fileName)
{
	parser = new Parser(fileName);
}

Automat::~Automat()
{
	if (parser) delete parser;
}

int Automat::WordsAutomat(char* s, char* startCondition, char* endCondition) const
{
	char* currentCondition = startCondition;
	for (int i = 1; i < static_cast<int>(strlen(s)) && IsLetter(s[i-1]); i++)
	{
		char* temp = new char[i];
		Copy(temp, s, i);
		currentCondition = parser->Find(currentCondition, temp);

		if (strcmp(currentCondition, endCondition) == 0)
		{
			return i;
		}

		currentCondition = startCondition;
	}

	return 0;
}

char* Automat::oneCharString(char c)
{
	char* temp = new char[2];
	temp[0] = c;
	temp[1] = '\0';
	return temp;
}

Automat::ParseResult* Automat::StringParse(char* s) const
{
	parser->ResetstartConditionI();
	char* startCondition = parser->GetStartCondition();

	//Перебираем все автоматы
	while (startCondition)
	{
		//Отдельный автомат для зарезервированных слов
		if (strcmp(startCondition, "isReservedWord") == 0)
		{
			const int lenght = WordsAutomat(s, startCondition, "reservedWord");
			if (lenght != 0)
			{
				char* word = new char[lenght];
				Copy(word, s, lenght);
				return new ParseResult(Token::ReservedWord, word);
			}
			startCondition = parser->GetStartCondition();
		}
		//Отдельный автомат для системных функций
		if (strcmp(startCondition, "isSysFunc") == 0)
		{
			const int lenght = WordsAutomat(s, startCondition, "sysFunc");
			if (lenght != 0)
			{
				char* word = new char[lenght];
				Copy(word, s, lenght);
				return new ParseResult(Token::SysFunction, word);
			}
			startCondition = parser->GetStartCondition();
		}
		//Отдельный автомат для стандартных типов данных
		if (strcmp(startCondition, "isReservedType") == 0)
		{
			const int lenght = WordsAutomat(s, startCondition, "reservedType");
			if (lenght != 0)
			{
				char* word = new char[lenght];
				Copy(word, s, lenght);
				return new ParseResult(Token::ReservedType, word);
			}
			startCondition = parser->GetStartCondition();
		}
		//Отдельный автомат для условных конструкций
		if (strcmp(startCondition, "isCondition") == 0)
		{
			const int lenght = WordsAutomat(s, startCondition, "condition");
			if (lenght != 0)
			{
				char* word = new char[lenght];
				Copy(word, s, lenght);
				return new ParseResult(Token::Condition, word);
			}
			startCondition = parser->GetStartCondition();
		}

		char* currentCondition = startCondition;
		int fragmentLenght = 0;

		//Для посимвольных автоматов
		for (int i = 0; i < static_cast<int>(strlen(s)); i++)
		{
			char* temp = oneCharString(s[i]);

			currentCondition = parser->Find(currentCondition, temp);


			if (strcmp(currentCondition, "error") != 0) fragmentLenght++;

			if (strcmp(currentCondition, "error0") == 0)
			{
				break;
			}

			if (strcmp(currentCondition, "error") == 0 || i == strlen(s) - 1)
			{
				if (fragmentLenght != 0)
				{
					char* word = new char[fragmentLenght];
					Copy(word, s, fragmentLenght);
					return new ParseResult(StrToToken(startCondition), word);
				}

				else
				{
					break;
				}
			}

			if (strcmp(currentCondition, "charError") == 0)
			{
				throw Exceptions::CharError;
			}

			if (strcmp(currentCondition, "stringerror") == 0)
			{
				throw Exceptions::StringError;
			}
		}

		startCondition = parser->GetStartCondition();
	}
	return nullptr;
}

List* Automat::Work(char* s) const
{
	List* tokens = new List(sizeof(ParseResult));

	while (strlen(s))
	{
		ParseResult* result = StringParse(s);

		tokens->add(result);

		char* temp = new char[strlen(s) - strlen(result->string) + 1];
		Copy(temp, s + strlen(result->string), strlen(s) - strlen(result->string));
		delete[] s;
		s = temp;
	}

	return tokens;
}
