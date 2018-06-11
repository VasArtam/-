#pragma once
#include "FileReader.h"
#include "FileWriter.h"
#include "List.h"

class Parser
{
private:
	FileReader* fileReader;
	FileWriter* fileWriter;

	List* parseConditions;
	List* startConditions;
	int startConditionI;

	struct Condition
	{
		char* before;
		char* after;
		char* word;

		Condition(char* before, char* after, char* word)
		{
			this->before = new char[strlen(before) + 1];
			this->after = new char[strlen(after) + 1];
			this->word = new char[strlen(word) + 1];

			Copy(this->before, before, strlen(before));
			Copy(this->after, after, strlen(after));
			Copy(this->word, word, strlen(word));
		}
	};

	void Add(char*, char*, char*) const;

	static List* Split(char*);
	static char* ThreeConcat(char*, char*, char*);

public:

	Parser(char*);
	~Parser();
	void StringParse(char*) const;
	char* Find(char*, char*) const;
	char* GetStartCondition();
	void ResetstartConditionI();
};