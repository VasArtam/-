#pragma once

//Используется в List.cpp
//ReSharper disable once CppUnusedIncludeDirective
#include "Heap.h"

#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	virtual ~List();

	//Возвращает указатель на конкретные данные, при вызове необходимо типизировать
	void*      get(int pos);
	//Push в конец списка, добавляет данные по указателю
	void       add(void* data);
	//Копирует элемент из одного места в другое
	void       copy_element(void* destination, void* source) const;

	//Извлечение элемента в store и исключение из списка
	void       take_first(void* store);
	void       take_last(void* store);
	void       take(int pos, void* store);
	//Вызывает compare
	void       sort(bool dir = true);

	int        count() const; //Текущее количество элементов в списке
	bool       call_error() const; //True, если возникла ошибка в последней операции

protected:
	virtual int compare(void* a, void* b) { return 0; }; //Переопределяется каждым дочерним классом, возвращает -1, 0, 1

private:
	struct Segment
	{
		void*    data; //Указывает на начало данных в сегменте
		Segment* prev; //Предыдущий сегмент
		Segment* next; //Следующий сегмент
	};
	Segment*     first; //Первый сегмент
	Segment*     last; //Последний сегмент
	int          first_index; //Номер первого элемента
	int          last_index; //Номер последнего элемента

	int          element_size; //Размер одного элемента в байтах
	int          element_count; //Количество элементов на один сегмент
	bool         error; //Возникла ли ошибка, например, при выделении памяти

	int          segment_count; //Текущее количество сегментов

	//Получает память из heap, создает новый сегмент
	void new_segment();
	//Возвращает память обратно, удаляет сегмент
	void delete_segment(Segment* seg);
};

