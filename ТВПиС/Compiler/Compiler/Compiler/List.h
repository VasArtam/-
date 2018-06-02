#pragma once

#include "heap.h"

#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	~List();

	//при вызове необходимо типизировать, возвращает указатель на конкретные данные
	void*      get(int pos);
	//извлечение и исключение из списка, возвращает указатель по номеру из списка
	void       add(void* data); //push в конец списка, добавляет данные по указателю
	//копирует элемент из одного места в другое
	void       copy_element(void* destination, void* source) const;

	//извлечение элемента в store и исключение из списка
	void       take_first(void* store); //начальный индекс слезает на 1 позицию вниз
	void       take_last(void* store); //конечный индекс слезает вверх. по аналогии
	void       take(int pos, void* store);
	//вызывает compare
	void       sort(bool dir = true);


	int        count() const;
	bool       call_error() const; // true if error in last operation
protected:
	virtual int compare(void* a, void* b) { return 0; }; //переопределяется каждым дочерним классом, возвращает -1, 0, 1

private:
	struct Segment
	{
		void*    data; //указывает на начало данных в сегменте
		Segment* prev; //предыдущий сегмент
		Segment* next; //следующий сегмент
	};
	Segment*     first; //первый сегмент
	Segment*     last; //последний сегмент
	int          first_index; //индекс первого элемента в первом блоке
	int          last_index; //последний занятый элемент в последнем блоке

	int          element_size; //размер одного элемента
	int          element_count; //текущее количество элементов
	bool         error; //возникла ли ошибка

	int          segment_count; //текущее количество сегментов

	void new_segment(); //получает память из heap
	void delete_segment(Segment* seg); //возвращает память обратно
};

