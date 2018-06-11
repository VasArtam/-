#include "stdafx.h"
#include "FileWriter.h"

void FileWriter::CloseFile() const
{
	if (file) file->close();
}

FileWriter::FileWriter(char* fileName)
{
	file = new ofstream();
	file->open(fileName);
}

FileWriter::~FileWriter()
{
	CloseFile();
}

void FileWriter::WriteLine(char* s) const
{
	*file << s << std::endl;
}
