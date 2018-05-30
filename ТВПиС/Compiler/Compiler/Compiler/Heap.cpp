#include <stdlib.h>
#include "heap.h"

using namespace std;

void* Heap::get_mem(int size)
{
	//Ищем, есть ли в предыдущих сегментах достаточно памяти для size
	//Поиск начинаем в обратном порядке: от текущего до первого
	Segment* s = current;

	while (s != nullptr)
	{
		int descriptor_count = s->descriptor_count;
		//Сначала ищем дескриптор с таким размером памяти, который нужен
		for (int i = 0; i < descriptor_count; i++)
		{
			Segment_def* descriptor = &s->descriptor[i];
			if (!descriptor->used && descriptor->size == size)
			{
				descriptor->used = true; //помечаем его занятым
				return descriptor->offset; //возвращаем указатель на область данных
			}
		}

		//Ищем блок с размером больше чем требуется, идем с конца, чтобы смещать меньше дескрипторов
		for (int i = descriptor_count - 1; i >= 0; i--)
		{
			Segment_def* descriptor = &s->descriptor[i];
			//Если нашли незанятый блок и его размер больше запрашиваемого размера
			if (!descriptor->used && descriptor->size > size)
			{
				//Если места на дескрипторы больше нет, то отдаем этот
				if (descriptor_count == DESCRIPTORCOUNT)
				{
					descriptor->used = true; //помечаем его занятым
				}
				else
				{
					split_descriptor(s, i, size);
				}
				
				return descriptor->offset; //возвращаем указатель на область данных
			}
		}

		//Если же в этом сегменте нет свободной памяти,
		//то повторим все то же самое для предыдущего сегмента
		s = s->prev;
	}

	//Если уж ни в каком сегменте нет свободной памяти,
	//то создадим новый
	make_segment();

	split_descriptor(current, 0, size);

	return current->descriptor[0].offset;
};

void Heap::split_descriptor(Heap::Segment* s, int position, int size)
{
	Segment_def* descriptor = &s->descriptor[position];
	//смещаем все дескрипторы того, который надо разделить, делаем место для нового
	for (int j = s->descriptor_count; j > position + 1; ++j)
	{
		s->descriptor[j] = s->descriptor[j - 1];
	}

	Segment_def* next_descriptor = &s->descriptor[position + 1];

	next_descriptor->size = descriptor->size - size;
	next_descriptor->offset = (void*)(int(descriptor->offset) + size);
	next_descriptor->used = false;

	descriptor->size = size;
	descriptor->used = true;

	//Увеличиваем количество дескрипторов
	++s->descriptor_count;
}


void Heap::free_mem(void* offset)
{
	//Ищем адрес, где нужно произвести освобождение
	//Поиск начинаем в обратном порядке: от текущего до первого
	Segment* s = current;

	while (s != nullptr)
	{
		//Если адрес находится между адресом начала сегмента и адресом его конца, то выполняем поиск в этом сегменте
		if (s > offset || s + segment_size <= offset)
		{
			//Если такого нет, то идем к предыдущему сегменту
			s = s->prev;
		}

		int descriptor_count = s->descriptor_count;
		//Ищем в дескрипторах адрес блока
		//Важно: если адрес будет попадать на сам дескриптор, ничего не произойдет
		for (int i = 0; i < descriptor_count; ++i)
		{
			Segment_def* descriptor = &s->descriptor[i];
			if (descriptor->offset != offset) continue;

			//Если мы нашли нужный адрес, производим освобождение
			descriptor->used = false;

			int movefrom = i;
			int moveto = i;
			Segment_def* prev_descriptor = &s->descriptor[i - 1];

			//Объединим соседние блоки, если какие-то из них пустые
			if (i != 0 && prev_descriptor->used == false)
			{
				prev_descriptor->size += descriptor->size;
				++movefrom;
			}

			Segment_def* next_descriptor = &s->descriptor[i + 1];
			if (i != descriptor_count - 1 && next_descriptor->used == false)
			{
				if (movefrom != i)
				{
					prev_descriptor->size += next_descriptor->size;
					++movefrom;
				}
				else
				{
					descriptor->size += next_descriptor->size;
					descriptor->used = false;
					movefrom += 2;
					++moveto;
				}
			}

			//смещаем дескрипторы
			if (moveto != movefrom)
			{
				while (movefrom < descriptor_count)
				{
					s->descriptor[moveto++] = s->descriptor[movefrom++];
				}

				s->descriptor_count -= movefrom - moveto;
			}


			//Если в сегменте больше не осталось занятых блоков
			//и это не последний оставшийся сегмент,
			//то удалим этот сегмент.

			bool empty = true;

			for (int j = 0; j < s->descriptor_count - 1; j++)
			{
				if (s->descriptor[j].used)
				{
					empty = false;
					break;
				}
			}

			if (empty)
			{
				current = s->prev;
				free(s);
			}

			return;
		}
	}
};

void Heap::make_segment()
{
	Segment* temp = (Segment*)malloc(segment_size); //Выделяем память под новый сегмент
	const unsigned descriptors_size = sizeof(Segment_def) * 1024;
	temp->data = temp + descriptors_size; //Указатель на часть с данными
	temp->prev = current; //Предыдущий сегмент - текущий

	Segment_def* first_descriptor = &temp->descriptor[0];
	//Задаем параметры первого блока
	first_descriptor->used = false;
	first_descriptor->size = segment_size - descriptors_size; //Размер всего сегмента минус размер массива дескрипторов
	first_descriptor->offset = temp->data; //Указатель на часть данными

	temp->descriptor_count = 1; //Количество блоков - 1

	current = temp; //Текущий становится новым
};

void Heap::delete_segments()
{
	while (true)
	{
		Segment* s = current;

		//Очищение данных в дескрипторах
		for (int i = 0; i < s->descriptor_count; i++)
		{
			s->descriptor[i].used = false;
			s->descriptor[i].size = 0;
			s->descriptor[i].offset = nullptr;
		}

		//Сегмент - последний
		if (current->prev == nullptr)
		{
			free(current);
			make_segment(); //создадим новый пустой
			return;
		}

		current = current->prev;
		free(s);
	}
};