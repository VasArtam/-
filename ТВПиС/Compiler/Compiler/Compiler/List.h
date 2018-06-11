#pragma once

//������������ � List.cpp
//ReSharper disable once CppUnusedIncludeDirective
#include "Heap.h"

#define LISTSIZE 64

class List
{
public:
	List(int element_size, int element_count = LISTSIZE);
	virtual ~List();

	//���������� ��������� �� ���������� ������, ��� ������ ���������� ������������
	void*      get(int pos);
	//Push � ����� ������, ��������� ������ �� ���������
	void       add(void* data);
	//�������� ������� �� ������ ����� � ������
	void       copy_element(void* destination, void* source) const;

	//���������� �������� � store � ���������� �� ������
	void       take_first(void* store);
	void       take_last(void* store);
	void       take(int pos, void* store);
	//�������� compare
	void       sort(bool dir = true);

	int        count() const; //������� ���������� ��������� � ������
	bool       call_error() const; //True, ���� �������� ������ � ��������� ��������

	void set_accessor(void*, int);
	void* get_accessor(int);
	void swap(int a, int b);

protected:
	virtual int compare(void* a, void* b) { return 0; }; //���������������� ������ �������� �������, ���������� -1, 0, 1

	struct Segment
	{
		void*    data;
		Segment* prev;
		Segment* next;
	};

	Segment* get_segment(int) const;

	bool is_free(Segment*) const;

private:
	Segment*     first; //������ �������
	Segment*     last; //��������� �������
	int          first_index; //����� ������� ��������
	int          last_index; //����� ���������� ��������

	int          element_size; //������ ������ �������� � ������
	int          element_count; //���������� ��������� �� ���� �������
	bool         error; //�������� �� ������, ��������, ��� ��������� ������

	int          segment_count; //������� ���������� ���������

	//�������� ������ �� heap, ������� ����� �������
	void new_segment();
	//���������� ������ �������, ������� �������
	void delete_segment(Segment* seg);
};

