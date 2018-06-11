#include "stdafx.h"
#include "Parser.h"
#include "List.h"
#include "FileReader.h"

#define var auto

Parser::Parser(char* fileName)
{
	startConditionI = 0;
	parseConditions = new List(sizeof(Condition));
	startConditions = new List(20);

	fileWriter = nullptr;//new FileWritter(LOG);

	fileReader = new FileReader(fileName);
	char* s;
	while ((s = fileReader->ReadLine()) != nullptr)
	{
		StringParse(s);
	}
	delete fileReader;
}

Parser::~Parser()
{
	if (parseConditions) delete parseConditions;
	if (startConditions) delete startConditions;
	if (fileWriter) delete fileWriter;
}

char* Parser::ThreeConcat(char* s1, char* s2, char* s3)
{
	char* temp = new char[strlen(s1) + strlen(s2) + strlen(s3) + 1];
	Copy(temp, s1, strlen(s1));
	Copy(temp + strlen(s1), "\t", 1);
	Copy(temp + strlen(s1) + 1, s2, strlen(s2));
	Copy(temp + strlen(s1) + 1 + strlen(s2), "\t", 1);
	Copy(temp + strlen(s1) + 1 + strlen(s2) + 1, s3, strlen(s3));

	return temp;
}


void Parser::Add(char* before, char* after, char* word) const
{
	const auto temp = new Condition(before, after, word);
	parseConditions->add(temp);
}

char* Parser::GetStartCondition()
{
	if (startConditionI < startConditions->count())
		return static_cast<char*>(startConditions->get_accessor(startConditionI++));
	
	return nullptr;
}

void Parser::ResetstartConditionI()
{
	startConditionI = 0;
}

List* Parser::Split(char* s)
{
	char* word = new char[20];
	List* wordList = new List(20); //Список разделенных слов

	//Оператор запятая
	if (strlen(s) == 1 && s[0] == ',')
	{
		word[0] = ',';
		word[1] = '\0';

		wordList->add(word);
		return wordList;
	}

	//Если escape-последовательность
	if (strlen(s) == 2 && s[0] == '\\')
	{
		if (s[1] == 'n') word[0] = '\n';
		if (s[1] == 't') word[0] = '\t';
		if (s[1] == '0') word[0] = '\0';
		word[1] = '\0';

		wordList->add(word);
		return wordList;
	}

	//Если несколько символов или слов, разделенных запятой
	int startPosition = 0;
	int i;
	for (i = 0; i < static_cast<int>(strlen(s)); i++)
	{
		if (s[i] == ',')
		{
			Copy(word, s + startPosition, i - startPosition);
			startPosition = i + 1;
			wordList->add(word);
		}
	}
	Copy(word, s + startPosition, i - startPosition);
	wordList->add(word);
	
	return wordList;
}

void Parser::StringParse(char* s) const
{
	if (strlen(s) == 0) return;
	if (s[0] == '#') return; //Комментарии
	if (strlen(s)>5)
	{
		char* start = new char[6];
		Copy(start, s, 5);
		
		if (!strcmp(start, "start"))
		{
			char* startCondition = new char[strlen(s) - 6];
			Copy(startCondition, s+6, strlen(s)-6);
			startConditions->add(startCondition);
			return;
		}
	}

	//Поиск позиций начала подстрок
	int positions[2];
	for (int i = 0, j = 0; i < static_cast<int>(strlen(s)) && j < 2; i++)
	{
		if (s[i] == '\t') positions[j++] = i + 1;
	}

	char* before = new char[positions[0]];
	char* words = new char[positions[1] - positions[0]];
	char* after = new char[strlen(s) - positions[1]];

	Copy(before, s, positions[0] - 1);
	Copy(words, s + positions[0], positions[1] - positions[0] - 1);
	Copy(after, s + positions[1], strlen(s) - positions[1]);

	List* wordList = Split(words);

	const int wordCount = wordList->count();
	for (int i = 0; i < wordCount; i++)
	{
		char* temp = static_cast<char*>(wordList->get(0));
		Add(before, after, temp);
	}

	delete wordList;
}

char* Parser::Find(char* condition, char* word) const
{
	for (int i = 0; i < parseConditions->count(); i++)
	{
		Condition* temp = static_cast<Condition*>(parseConditions->get_accessor(i));
		if (strcmp(temp->before, condition) == 0 && strcmp(temp->word, word) == 0)
		{
			char* writeString = ThreeConcat(temp->before, temp->word, temp->after);

			if (fileWriter) fileWriter->WriteLine(writeString);

			return temp->after;
		}
		if (strcmp(temp->before, condition) == 0 && strcmp(temp->word, "...") == 0)
		{
			char* writeString = ThreeConcat(temp->before, word, temp->after);

			if (fileWriter) fileWriter->WriteLine(writeString);

			return temp->after;
		}
	}
	return nullptr;
}
