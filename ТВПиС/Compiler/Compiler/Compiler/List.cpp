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
	//Удаляем все сегменты от первого до последнего
	Segment* i = first;
	while (i)
	{
		Segment* temp = i;
		delete_segment(i);
		i = temp->next;
	}
	//Зануляем указатели
	first = last = nullptr;
	//Зануляем все переменные
	first_index = last_index = element_size = element_count = segment_count = 0;
}

void* List::get(int pos)
{
	//Если элементов нет
	if (count() == 0) return nullptr;

	//Выделим памяти на один элемент
	void* data = Heap::Instance().get_mem(sizeof element_size);

	//Возьмем элемент под определенным номером и запишем данные из него в data
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
	//Если передан указатель на nullptr, бросим exception
	if (data == nullptr)
		throw new bad_alloc;

	//Текущее количество сегментов
	int segment_number = 0;
	if (last_index >= element_count) segment_number = last_index / element_count;
	const int cell = last_index % element_count;

	Segment* segment = nullptr;

	//Если cell равно нулю, создадим новый сегмент, иначе получим сегмент с номером segment_number
	//Segment* segment = (cell == 0) ? NewSegment() : GetSegment(segmentNumber);
	//char* offset = (char*)segment->data + (cell * elementSize);
	if (cell == 0)
	{
		new_segment();
		segment = this->last;
	}
	else
	{
		//Получение нужного сегмента в segment
		segment = get_segment(segment_number);
	}

	//Если сегмент остался нулевым по каким-то причинам, закончим функцию ничего не сделав
	if (segment == nullptr)
	{
		return;
	}

	//Зададим offset и скопируем туда данные для добавления
	void* offset = static_cast<char*>(segment->data) + cell * element_size;

	copy_element(static_cast<char*>(segment->data) + cell * element_size, data);

	//Увеличим последний индекс на 1
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
	//Если элементов нет
	if (count() == 0)
	{
		store = nullptr;
		return;
	}
	//Если передали первую позицию, то лучше выполним другую функцию
	if (pos == 0)
	{
		take_first(store);
		return;
	}
	//Если передали последнюю позицию, то лучше выполним другую функцию
	//if (pos == first_index + last_index - 1)
	if (pos == last_index - first_index + 1)
	{
		take_last(store);
		return;
	}

	//К позиции прибавляем индекс первого элемента - 1, теперь это нужный нам индекс
	pos += first_index - 1;

	//Определение сегмента, в котором находится элемент с данным номером позиции
	Segment* i = first;
	int segment_number = 0;
	//Номер нужного сегмента считается как целая часть от деления
	//индекса элемента на количество элементов на один сегмент
	const int segment_id = pos / element_count;

	//Если получилось больше, чем количество сегментов,
	//или отрицательное число, то дропнем выполнение функции
	if (segment_id > segment_count || segment_id < 0)
	{
		error = true;
		return;
	}
	//Ищем сегмент
	while (segment_number != segment_id)
	{
		i = i->next;
		segment_number++;
	}
	//Текущий сегмент
	Segment* current_segment = i;

	//Индекс элемента в текущем сегменте
	int element_index = pos % element_count;
	//Найдем, откуда копировать память
	char* source = static_cast<char*>(current_segment->data) + element_index * element_size;
	//Скопиурем необходимые данные в место для хранения
	copy_element(store, source);

	//Сместим все элементы, которые были после pos, на одну позицию вниз
	while (pos != last_index)
	{
		char* destination = static_cast<char*>(current_segment->data) + element_index * element_size;

		//Если индекс - не последний для этого сегмента
		if (element_index < element_count - 1)
			//То будем копировать память из следующего элемента
			source = static_cast<char*>(current_segment->data) + (element_index + 1) * element_size;
		else
			//Иначе будем копировать память из первого элемента следующего сегмента
			source = static_cast<char*>(current_segment->next->data);

		//Копируем на одну позицию вниз
		copy_element(destination, source);
		//Увеличиваем индекс и позицию
		element_index++;
		pos++;

		//Если индекс - последний, то сделаем его 0 и перейдем в следующий сегмент
		if (element_index == element_count - 1)
		{
			current_segment = current_segment->next;
			element_index = 0;
		}
	}
	last_index--;

	//Проверка, пустой ли последний сегмент
	i = first;
	int j = 0; //Номер последнего сегмента

	while (i)
	{
		j++;
		i = i->next;
	}
	//Если номер последнего элемента меньше или равен общему количеству элементов,
	//которые могут поместиться в текущем количестве сегментов - 1, то удалим последний сегмент
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