#include "stdafx.h"
#include "Segment.h"
#include "Heap.h"
#include <exception>

using namespace std;


Heap::Heap(int segmentSize)  //�����������, �������� �� ���� ������ ��������.
{
	this->segmentSize = segmentSize;  //���������� �������� �������� ������� �������� � ��������������� ���������� ������
	this->current = nullptr;
	this->current = MakeSegment();   //��� current ������� ����� ������� ������� MakeSegment
}

Heap& Heap::Instance(int segmentSize)
{
	static Heap _instance (segmentSize);
	return _instance;
}

Heap::~Heap(void)  //����������
{
	DeleteSegments();    //������� ��� ��������
	current = nullptr;   //�������� ������ � current
}

void* Heap::GetMemory(int size)  //��������� ������. �� ���� ��������� ������ �������������� ����� � ������
{
	if (size > this->segmentSize || size < 0)   //���� ������������� ������ ������ ��� ������ �������� ��� ������ ����, �� ����������� ������
	{
		throw new bad_alloc();
	}
	Segment* i = current;    //�������� ������� ������� ��� ���������� ������

	while (i)    //���� ��� �� ���� ���������
	{
		void* newBlock = i->PushBlock(size);    //������� �������� ���� � ������� i
		if (newBlock != nullptr)     //���� �������� ���� �������
			return newBlock;  //�� ���������� ���� ����
		i = i->prev;      //� ��������� ������ ��������� � ���������� ��������
	}

	try   //���� ���� ��� � �� ��������� �� � ���� �������, �� ������� ����� �������
	{
		i = MakeSegment();
	}
	catch (std::bad_alloc e)
	{
		throw new bad_alloc;    //���� �� ������� �������, ����������� ����������
	}
	i->PushBlock(size);     //� ��� � ���� ����� �������� ���������� ��� ����. ��� �������� ����� ����������

	return i;     //���������� �������
}

void Heap::FreeMemory(void* offset)    //����������� ������ � ������������ ������
{
	current->RemoveBlock(offset);      //��� �������� �������� �������� RemoveBlock �� �������� ������ �� ��������� offset
}

void Heap::FreeMemory(Segment* segment)  //����������� ������ �� ��� ��������
{
	Segment* i = current;    //����� ������� �������
	Segment* prev = nullptr;    //����� ������� � ������� �������
	while (i != segment)     //����� �� ������� ��� ��������. i-��� �������, prev-������� ����� ���
	{
		prev = i;
		i = i->prev;
	}
	delete i;   //������� ������ �������
	if (prev)   //���� ������� ����� ��������� �����������, ����� �������������� ��� ������ ���������������� ���������
        prev->prev = i->prev;
}

Segment* Heap::MakeSegment()    //������� �������
{
	Segment* temp = new Segment(current, segmentSize);     //�������� ����������� ��������,� ������� �������� ������� ������� � ������ ��������
	current = temp;  //�������� �������� ����������� ��������� �������
	return current;    //���������� ������� ������� (������ ��� ���������)
}

void Heap::DeleteSegments()  //������� �������� ���
{
	Segment* i = current;   //����� ������� �������
	while (i)    //���� �������� ����������
	{
		Segment* temp = i;  //���������� �������� ����������� ������� i
		i = i->prev;   //i ����������� ���������� �������
		delete temp;  //��������� ������� �������
	}

}
