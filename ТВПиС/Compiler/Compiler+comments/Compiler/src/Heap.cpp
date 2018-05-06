#include "stdafx.h"
#include "Segment.h"
#include "Heap.h"
#include <exception>

using namespace std;


Heap::Heap(int segmentSize)  //конструктор, получает на вход размер сегмента.
{
	this->segmentSize = segmentSize;  //записывает желаемое значение размера сегмента в соответствующую переменную класса
	this->current = nullptr;
	this->current = MakeSegment();   //для current создает новый сегмент вызовом MakeSegment
}

Heap& Heap::Instance(int segmentSize)
{
	static Heap _instance (segmentSize);
	return _instance;
}

Heap::~Heap(void)  //деструктор
{
	DeleteSegments();    //удаляет все сегменты
	current = nullptr;   //обнуляет ссылку в current
}

void* Heap::GetMemory(int size)  //выделение памяти. на вход поступает размер запрашиваемого места в памяти
{
	if (size > this->segmentSize || size < 0)   //если запрашиваемая память больше чем размер сегмента или меньше нуля, то выбрасываем ошибку
	{
		throw new bad_alloc();
	}
	Segment* i = current;    //выбираем текущий сегмент для дальнейшей работы

	while (i)    //пока идём по всем сегментам
	{
		void* newBlock = i->PushBlock(size);    //пробуем впихнуть блок в сегмент i
		if (newBlock != nullptr)     //если впихнуть блок удалось
			return newBlock;  //то возвращаем этот блок
		i = i->prev;      //в противном случае переходим к следующему сегменту
	}

	try   //если блок так и не впихнулся ни в один сегмент, то создаем новый сегмент
	{
		i = MakeSegment();
	}
	catch (std::bad_alloc e)
	{
		throw new bad_alloc;    //если не удалось создать, выбрасываем исключение
	}
	i->PushBlock(size);     //и уже в этот новый сегммент запихиваем наш блок. так впихнуть точно получиться

	return i;     //возвращаем сегмент
}

void Heap::FreeMemory(void* offset)    //освобождаем память в определенной ячейке
{
	current->RemoveBlock(offset);      //для текущего сегмента вызываем RemoveBlock на удаление ячейки со смещением offset
}

void Heap::FreeMemory(Segment* segment)  //освобождаем память во всём сегменте
{
	Segment* i = current;    //берем текущий сегмент
	Segment* prev = nullptr;    //берем сегмент с нулевой ссылкой
	while (i != segment)     //дошли до нужного нам сегмента. i-сам сегмент, prev-сегмент перед ним
	{
		prev = i;
		i = i->prev;
	}
	delete i;   //удалили нужный сегмент
	if (prev)   //если сегмент перед удаляемым существовал, тогда предшествующим ему делаем предшествовавший удалённому
        prev->prev = i->prev;
}

Segment* Heap::MakeSegment()    //создаем сегмент
{
	Segment* temp = new Segment(current, segmentSize);     //вызываем конструктор сегмента,в котором передаем текущий сегмент и размер сегмента
	current = temp;  //текущему сегменту присваиваем созданный сегмент
	return current;    //возвращаем текущий сегмент (только что созданный)
}

void Heap::DeleteSegments()  //удаляем сегменты все
{
	Segment* i = current;   //берем текущий сегмент
	while (i)    //пока сегменты существуют
	{
		Segment* temp = i;  //временному сегменту пресваиваем сегмент i
		i = i->prev;   //i присваиваем предыдущий сегмент
		delete temp;  //временный сегмент удаляем
	}

}
