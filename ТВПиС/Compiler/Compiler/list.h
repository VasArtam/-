#include "heap.h"

#define LISTSIZE 64

class List
{
public:
	List(int _element_size, int _element_count = LISTSIZE);
	~List();

	//при вызове необходимо типизировать, возвращает указатель на конкретные данные
	void*      get(int pos);
	void       add(void* data);

	//returns and deletes elements
	void       take_first(void* store); //начальный индекс слезает на 1 позицию вниз. если сделать достаточное количество раз, можно опустошить сегмент
	void       take_last(void* store); // конечный индекс слезает вверх. по аналогии
	void       take(int pos, void* store);
	//вызывает compare
	void       sort(bool dir = true);


        int        count();
        bool       error(); { return error; } // true if error in last operation
protected:
    virtual int compare (void* a, void* b) { return 0; }; //переопределяется каждым дочерним классом, возвращает -1, 0, 1

private:
	struct Segment
	{
		void*    data; //указывает на начало блока
		Segment* prev; //предыдущий блок
		Segment* next; //ледующий блок
	};
	Segment*         first; //первый сегмент
	Segment*         last; //последний сегмент
	int              first_index; //индекс первого элемента в первом блоке
	int              last_index; //последний занятый элемент в последнем блоке

	int              element_size;
	int              element_count;
    bool             error;

	void new_segment(); //получает память из heap
	void delete_segment(Segment* seg); //возвращает память обратно
};

