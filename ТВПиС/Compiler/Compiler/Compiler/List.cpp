#include "stdafx.h"
#include "List.h"
#include <exception>

using namespace std;

List::List(int elementSize, int elementCount)
{
	if (elementSize <= 0 || elementCount <= 0) throw new bad_alloc();

	this->element_size = elementSize;
	this->element_count = elementCount;

	this->error = false;

	this->segment_count = 0;

	this->first = this->last = nullptr;
	this->first_index = this->last_index = 0;
}

List::~List()
{
	Segment* i = first;
	while (i)
	{
		delete_segment(i);
		i = i->next;
	}
	first = last = nullptr;
	first_index = last_index = 0;
	element_size = element_count = 0;
	segment_count = 0;
}

void* List::get(int pos)
{
	if (count() == 0) return nullptr;
	void* data = Heap::Instance().get_mem(sizeof(element_size));
	take(pos, data);
	return data;
}

List::Segment* get_segment(int id)
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

void List::copy_element(void* destination, void* source) const
{
	for (int i = 0; i < element_size; i++)
		*(static_cast<char*>(destination) + i) = *(static_cast<char*>(source) + i);
}

void List::add(void* data)
{
	if (data == nullptr)
		throw new bad_alloc;

	const int segmentNumber = last_index / element_count;
	const int cell = last_index % element_count;

	Segment* segment = (cell == 0) ? new_segment() : get_segment(segmentNumber);

	char* offset = static_cast<char*>(segment->data) + (cell * element_size);

	copy_element(offset, data);

	last_index++;

	return offset;
}

void List::take_first(void* store)
{
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	char* source = (char*)first->data + first_index * element_size;
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
	char* source = (char*)last->data + ((last_index - 1) % element_count)*element_size;
	copy_element(store, source);
	last_index--;

	if (last_index % element_count == 0)
	{
		delete_segment(last);
	}
}

void List::take(int pos, void* store)
{
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	if (pos == 0)
	{
		take_first(store);
		return;
	}
	if (pos == first_index + last_index - 1)
	{
		take_last(store);
		return;
	}

	pos += first_index;
	Segment* currentSegment = get_segment(pos / element_count);
	int elementIndex = pos % element_count;

	char* source = (char*)currentSegment->data + elementIndex * element_size;
	copy_element(store, source);

	while (pos != last_index)
	{
		char* destination = static_cast<char*>(currentSegment->data) + elementIndex * element_size;

		if (elementIndex < element_count - 1)
			source = static_cast<char*>(currentSegment->data) + (elementIndex + 1)*element_size;
		else
			source = static_cast<char*>(currentSegment->next->data);

		copy_element(destination, source);
		elementIndex++;
		pos++;

		if (elementIndex == element_count)
		{
			currentSegment = currentSegment->next;
			elementIndex = 0;
		}
	}

	bool free = false;

	//проверка, пустой ли текущий сегмент
	Segment* i = first;
	int j = 0;
	while (i)
	{
		j++;
		i = i->next;
	}

	if (last_index /*+ firstIndex*/ <= (j - 1)*element_count) free = true;

	if (free) delete_segment(currentSegment);


	last_index--;
}

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
	return temp;
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

/*void* List::GetAccessor(int n)
{
	n += first_index;

	if (n >= last_index)
		throw Exceptions::ArgumentOutOfRange;

	Segment* currentSegment = GetSegment(n / element_count);
	int elementIndex = n % element_count;

	char* source = (char*)currentSegment->data + elementIndex * element_size;
	return source;
}

void List::SetAccessor(void* data, int n)
{
	n += first_index;
	Segment* currentSegment = GetSegment(n / element_count);
	int elementIndex = n % element_count;

	char* source = (char*)currentSegment->data + elementIndex * element_size;
	copy_element(source, data);
}*/

void List::swap(int a, int b)
{
	void* temp = Heap::Instance().get_mem(element_size);
	copy_element(temp, GetAccessor(a));
	SetAccessor(GetAccessor(b), a);
	SetAccessor(temp, b);
}