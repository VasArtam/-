#pragma once
#include <fstream>

using namespace std;

class FileReader
{
private:
	ifstream * file;
	void OpenFile(char*) const;
	void CloseFile() const;

public:
	FileReader(char*);
	~FileReader();
	char* ReadLine() const;
	char* ReadFile() const;
	char* ReadSourceCode() const;
};

