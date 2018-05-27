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
		int memory = 0;

		//Подсчитаем свободную память
		for (int i = 0; i < s->descriptor_count; i++)
		{
			if (!s->descriptor[i].used)
			{
				memory += s->descriptor[i].size;
			}
		}

		//Если свободной памяти больше, чем требуется (не факт, что найдется место)
		if (memory >= size)
		{
			//Ищем первый незанятый блок
			for (int i = 0; i < s->descriptor_count; i++)
			{
				//Если нашли незанятый блок и его размер равен запрашиваемому размеру
				if (!s->descriptor[i].used && s->descriptor[i].size == size)
				{
					s->descriptor[i].used = true; //помечаем его занятым
					return s->descriptor[i].offset; //возвращаем указатель на него
				}
				//Если нашли незанятый блок и его размер больше запрашиваемого размера
				else if (!s->descriptor[i].used && s->descriptor[i].size > size)
				{
					//Большой брат = блок, который делят
					//Маленький брат = новый блок

					//Маленький брат занимает место большого брата
					s->descriptor[s->descriptor_count].offset = s->descriptor[i].offset;
					//Размер маленького брата - размер запрашиваемой памяти
					s->descriptor[s->descriptor_count].size = size;
					//Маленький брат используется
					s->descriptor[s->descriptor_count].used = true;

					//Увеличиваем количество дескрипторов
					s->descriptor_count++;

					//Большой брат переезжает вперед на размер запрашиваемой памяти
					s->descriptor[i].offset = (char*)s->descriptor[i].offset + size;
					//Размер большого брата становится меньше
					s->descriptor[i].size -= size;
					//Большой брат не используется
					s->descriptor[i].used = false;

					return s->descriptor[s->descriptor_count].offset;
				}
			}
		}
		//Если же в этом сегменте нет свободной памяти,
		//то повторим все то же самое для предыдущего сегмента
		s = s->prev;
	}

	//Если уж ни в каком сегменте нет свободной памяти,
	//то создадим новый
	make_segment();

	s = current;

	//Маленький брат занимает место большого брата
	s->descriptor[s->descriptor_count].offset = s->descriptor[0].offset;
	//Размер маленького брата - размер запрашиваемой памяти
	s->descriptor[s->descriptor_count].size = size;
	//Маленький брат используется
	s->descriptor[s->descriptor_count].used = true;

	//Увеличиваем количество дескрипторов
	s->descriptor_count++;

	//Большой брат переезжает вперед на размер запрашиваемой памяти
	s->descriptor[0].offset = (char*)s->descriptor[0].offset + size;
	//Размер большого брата становится меньше
	s->descriptor[0].size -= size;
	//Большой брат не используется
	s->descriptor[0].used = false;

	return current->descriptor[0].offset;
};

void Heap::free_mem(void* offset)
{
	//Ищем адрес, где нужно произвести освобождение
	//Поиск начинаем в обратном порядке: от текущего до первого
	Segment* s = current;

	while (s != nullptr)
	{
		//Если адрес находится между адресом начала сегмента и адресом его конца, то выполняем поиск в этом сегменте
		if (s <= offset && s + sizeof(s) > offset)
		{
			//Ищем в дескрипторах адрес блока
			//Важно: если адрес будет попадать на сам дескриптор, ничего не произойдет
			int i = 0;
			while (i < s->descriptor_count - 1)
			{
				//Если мы нашли нужный адрес, производим освобождение
				if (s->descriptor[i].offset == offset)
				{
					s->descriptor[i].used = false;

					free(s->descriptor[i].offset);

					//Объединим соседние блоки, если какие-то из них пустые
					if (i >= 1 && s->descriptor[i - 1].used == false)
					{
						s->descriptor[i - 1].size += s->descriptor[i].size;
						s->descriptor[i].size = 0;
						s->descriptor[i].offset = s->descriptor[i - 1].offset;
					}
					if (i <= s->descriptor_count - 1 && s->descriptor[i + 1].used == false)
					{
						s->descriptor[i].size += s->descriptor[i + 1].size;
						s->descriptor[i + 1].size = 0;
						s->descriptor[i + 1].offset = s->descriptor[i].offset;
					}

					//Если в сегменте больше не осталось занятых блоков
					//и это не последний оставшийся сегмент,
					//то удалим этот сегмент.

					bool empty = true;

					for (int j = 0; i < s->descriptor_count - 1; i++)
					{
						if (s->descriptor[i].used = true) break;
					}

					if (empty)
					{
						//delete_segment(s);
					}

					return;
				}
				//Если нет - то идем к следующему адресу
				i++;
			}
		}
		//Если такого нет, то идем к предыдущему сегменту
		s = s->prev;
	}
};

void Heap::make_segment()
{
	Segment* temp = (Segment*)malloc(segment_size); //Выделяем память под новый сегмент
	temp->data = temp + sizeof(Segment_def) * 1024; //Указатель на часть с данными
	temp->prev = current; //Предыдущий сегмент - текущий

						  //Задаем параметры первого блока
	temp->descriptor[0].used = false;
	temp->descriptor[0].size = sizeof(temp) - sizeof(Segment_def) * 1024; //Размер всего сегмента минус размер массива дескрипторов
	temp->descriptor[0].offset = temp->data; //Указатель на часть данными

	temp->descriptor_count = 1; //Количество используемых блоков - 1

	current = temp; //Текущий становится новым
};

void Heap::delete_segments()
{
	Segment* temp;
	while (current != 0)
	{
		if (current->prev == 0 || current->prev == nullptr)
		{
			//temp = current.prev;
			free(current->data);
			temp = nullptr;
		}
		else
		{
			temp = current->prev;
			current = nullptr;
		}
		current = nullptr;
		current = temp;
	}
};