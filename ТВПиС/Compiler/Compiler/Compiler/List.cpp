#include "stdafx.h"
#include "List.h"
#include <exception>
#include "Exceptions.h"

using namespace std;

List::List(const int element_size, int element_count)
{
	if (element_size <= 0 || element_count <= 0) throw new bad_alloc();

	this->element_size = element_size;
	this->element_count = element_count;

	this->error = false;

	this->segment_count = 0;

	this->first = this->last = nullptr;
	this->first_index = this->last_index = 0;
}

List::~List()
{
	//������� ��� �������� �� ������� �� ����������
	Segment* i = first;
	while (i)
	{
		Segment* temp = i;
		delete_segment(i);
		i = temp->next;
	}
	//�������� ���������
	first = last = nullptr;
	//�������� ��� ����������
	first_index = last_index = element_size = element_count = segment_count = 0;
}

void* List::get(int pos)
{
	//���� ��������� ���
	if (count() == 0) return nullptr;

	//������� ������ �� ���� �������
	void* data = Heap::Instance().get_mem(sizeof element_size);

	//������� ������� ��� ������������ ������� � ������� ������ �� ���� � data
	take(pos, data);

	return data;
}

List::Segment* List::get_segment(int id) const
{
	Segment* i = first;
	int n = 0;
	if (id > segment_count || id < 0) return nullptr;

	while (n != id)
	{
		i = i->next;
		n++;
	}
	return i;
}

bool List::is_free(Segment* segment) const
{
	Segment* i = first;
	int j = 0;
	while (i)
	{
		j++;
		i = i->next;
	}

	if (last_index /*+ firstIndex*/ <= (j - 1) * element_count)
		return true;

	return false;
}

void List::copy_element(void* destination, void* source) const
{
	for (int i = 0; i < element_size; i++)
		*(static_cast<char*>(destination) + i) = *(static_cast<char*>(source) + i);
}

void List::add(void* data)
{
	//���� ������� ��������� �� nullptr, ������ exception
	if (data == nullptr)
		throw new bad_alloc;

	//������� ���������� ���������
	int segment_number = 0;
	if (last_index >= element_count) segment_number = last_index / element_count;
	const int cell = last_index % element_count;

	Segment* segment = nullptr;

	//���� cell ����� ����, �������� ����� �������, ����� ������� ������� � ������� segment_number
	//Segment* segment = (cell == 0) ? NewSegment() : GetSegment(segmentNumber);
	//char* offset = (char*)segment->data + (cell * elementSize);
	if (cell == 0)
	{
		new_segment();
		segment = this->last;
	}
	else
	{
		//��������� ������� �������� � segment
		segment = get_segment(segment_number);
	}

	//���� ������� ������� ������� �� �����-�� ��������, �������� ������� ������ �� ������
	if (segment == nullptr)
	{
		return;
	}

	//������� offset � ��������� ���� ������ ��� ����������
	void* offset = static_cast<char*>(segment->data) + cell * element_size;

	copy_element(static_cast<char*>(segment->data) + cell * element_size, data);

	//�������� ��������� ������ �� 1
	if (segment_count > 0) last_index++;
}

void List::take_first(void* store)
{
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	char* source = static_cast<char*>(first->data) + first_index * element_size;
	copy_element(store, source);
	first_index++;

	if (first_index == element_count)
	{
		first_index -= element_count;
		last_index -= element_count;
		delete_segment(first);
	}
}

void List::take_last(void* store)
{
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	char* source = static_cast<char*>(last->data) + (last_index - 1) % element_count*element_size;
	copy_element(store, source);
	last_index--;

	if (last_index % element_count == 0)
	{
		delete_segment(last);
	}
}

void List::take(int pos, void* store)
{
	//���� ��������� ���
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	//���� �������� ������ �������, �� ����� �������� ������ �������
	if (pos == 0)
	{
		take_first(store);
		return;
	}
	//���� �������� ��������� �������, �� ����� �������� ������ �������
	//if (pos == first_index + last_index - 1)
	if (pos == last_index - first_index + 1)
	{
		take_last(store);
		return;
	}

	//� ������� ���������� ������ ������� �������� - 1, ������ ��� ������ ��� ������
	pos += first_index - 1;

	//����������� ��������, � ������� ��������� ������� � ������ ������� �������
	Segment* i = first;
	int segment_number = 0;
	//����� ������� �������� ��������� ��� ����� ����� �� �������
	//������� �������� �� ���������� ��������� �� ���� �������
	const int segment_id = pos / element_count;

	//���� ���������� ������, ��� ���������� ���������,
	//��� ������������� �����, �� ������� ���������� �������
	if (segment_id > segment_count || segment_id < 0)
	{
		error = true;
		return;
	}
	//���� �������
	while (segment_number != segment_id)
	{
		i = i->next;
		segment_number++;
	}
	//������� �������
	Segment* current_segment = i;

	//������ �������� � ������� ��������
	int element_index = pos % element_count;
	//������, ������ ���������� ������
	char* source = static_cast<char*>(current_segment->data) + element_index * element_size;
	//��������� ����������� ������ � ����� ��� ��������
	copy_element(store, source);

	//������� ��� ��������, ������� ���� ����� pos, �� ���� ������� ����
	while (pos != last_index)
	{
		char* destination = static_cast<char*>(current_segment->data) + element_index * element_size;

		//���� ������ - �� ��������� ��� ����� ��������
		if (element_index < element_count - 1)
			//�� ����� ���������� ������ �� ���������� ��������
			source = static_cast<char*>(current_segment->data) + (element_index + 1) * element_size;
		else
			//����� ����� ���������� ������ �� ������� �������� ���������� ��������
			source = static_cast<char*>(current_segment->next->data);

		//�������� �� ���� ������� ����
		copy_element(destination, source);
		//����������� ������ � �������
		element_index++;
		pos++;

		//���� ������ - ���������, �� ������� ��� 0 � �������� � ��������� �������
		if (element_index == element_count - 1)
		{
			current_segment = current_segment->next;
			element_index = 0;
		}
	}
	last_index--;

	//��������, ������ �� ��������� �������
	i = first;
	int j = 0; //����� ���������� ��������

	while (i)
	{
		j++;
		i = i->next;
	}
	//���� ����� ���������� �������� ������ ��� ����� ������ ���������� ���������,
	//������� ����� ����������� � ������� ���������� ��������� - 1, �� ������ ��������� �������
	if (last_index + 1 <= (j - 1) * element_count)
	{
		delete_segment(last);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
void List::sort(bool dir) {}

int List::count() const
{
	return last_index - first_index;
}

bool List::call_error() const
{
	return error;
}

void List::new_segment()
{
	Segment* temp = new Segment();

	if (first == nullptr)
	{
		first = last = temp;
		first->next = first->prev = nullptr;
	}
	else
	{
		last->next = temp;
		temp->prev = last;
		last = temp;
	}

	try
	{
		temp->data = Heap::Instance().get_mem(element_count*element_size);
	}
	catch (bad_alloc)
	{
		error = true;
		return;
	}

	segment_count++;
}

void List::delete_segment(Segment* segment)
{
	if (segment == first) first = first->next;
	if (segment == last) last = last->prev;

	if (segment->prev) segment->prev->next = segment->next;
	if (segment->next) segment->next->prev = segment->prev;
	Heap::Instance().free_mem(segment->data);
	segment_count--;
}

void* List::get_accessor(int n)
{
	n += first_index;

	if (n >= last_index)
		throw ArgumentOutOfRange;

	Segment* currentSegment = get_segment(n / element_count);
	const int element_index = n % element_count;

	char* source = static_cast<char*>(currentSegment->data) + element_index * element_size;
	return source;
}

void List::set_accessor(void* data, int n)
{
	n += first_index;
	Segment* currentSegment = get_segment(n / element_count);
	const int element_index = n % element_count;

	char* source = static_cast<char*>(currentSegment->data) + element_index * element_size;
	copy_element(source, data);
}

void List::swap(int a, int b)
{
	void* temp = Heap::Instance().get_mem(element_size);
	copy_element(temp, get_accessor(a));
	set_accessor(get_accessor(b), a);
	set_accessor(temp, b);
}