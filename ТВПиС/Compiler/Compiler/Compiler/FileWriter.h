#pragma once
#include <fstream>

using namespace std;

class FileWriter
{
private:
	ofstream * file;

	void CloseFile() const;

public:
	FileWriter(char*);
	~FileWriter();

	void WriteLine(char*) const;
};

