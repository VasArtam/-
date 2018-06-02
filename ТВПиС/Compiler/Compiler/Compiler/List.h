#pragma once

#include "heap.h"

#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	~List();

	//��� ������ ���������� ������������, ���������� ��������� �� ���������� ������
	void*      get(int pos);
	//���������� � ���������� �� ������, ���������� ��������� �� ������ �� ������
	void       add(void* data); //push � ����� ������, ��������� ������ �� ���������
	//�������� ������� �� ������ ����� � ������
	void       copy_element(void* destination, void* source) const;

	//���������� �������� � store � ���������� �� ������
	void       take_first(void* store); //��������� ������ ������� �� 1 ������� ����
	void       take_last(void* store); //�������� ������ ������� �����. �� ��������
	void       take(int pos, void* store);
	//�������� compare
	void       sort(bool dir = true);


	int        count() const;
	bool       call_error() const; // true if error in last operation
protected:
	virtual int compare(void* a, void* b) { return 0; }; //���������������� ������ �������� �������, ���������� -1, 0, 1

private:
	struct Segment
	{
		void*    data; //��������� �� ������ ������ � ��������
		Segment* prev; //���������� �������
		Segment* next; //��������� �������
	};
	Segment*     first; //������ �������
	Segment*     last; //��������� �������
	int          first_index; //������ ������� �������� � ������ �����
	int          last_index; //��������� ������� ������� � ��������� �����

	int          element_size; //������ ������ ��������
	int          element_count; //������� ���������� ���������
	bool         error; //�������� �� ������

	int          segment_count; //������� ���������� ���������

	void new_segment(); //�������� ������ �� heap
	void delete_segment(Segment* seg); //���������� ������ �������
};

