#include "stdafx.h"
#include "List.h"
#include <exception>

using namespace std;

List::List(int elementSize, int elementCount)  //конструктор листа, принимает размер элементов и количество
{
    if (elementSize <= 0 || elementCount <= 0)    //если размер или количество меньше нуля, то выбрасываем исключение
        throw new bad_alloc();
    this->elementSize = elementSize;  //закладываем размер элемента
    this->elementCount = elementCount; //закладываем количество элементов
    this->error = false;  //закладываем отсутствие ошибки
    this->segmentCount = 0;  //обнуляем количество сегментов
    this->first = this->last = nullptr; //обнуляем первый и последний сегмент
    this->firstIndex = this->lastIndex = 0;  //обнуляем номер первого и последнего элемента
}

List::~List()  //деструктор листа
{
    Segment* i = first;  //берем первый сегмент
    while (i)   //пока идём по сегментам
    {
        DeleteSegment(i);  //вызываем уаление сегмента
        i=i->next; //переходим к следующему сегменту
    }
    first = last = nullptr;  //обнуляем ссылку на первый и последний сегмент
    firstIndex = lastIndex = 0; //обнуляем номер первого и последнего элемента
    elementSize = elementCount = 0;  //обнуляем количество элементов и размер элемента
    segmentCount = 0;   //обнуляем количество сегментов
}

void* List::Get(int pos)  //извлечение из списка, на вход подается позиция извлекаемого элмента
{
    if (Count()==0)  //если извлекать нечего возвращаем нулевую ссылку
        return nullptr;
    void* data = Heap::Instance().GetMemory(sizeof(elementSize));  //выделяем через хип место под извлекаемый элемент
    Take(pos, data);  //извлекаем элемент
    return data;  //возвращаем ссылку в хипе на элемент
}

List::Segment* List::GetSegment(int id)   //получение сегмент, на вход подается айди нужного сегмента
{
    Segment* i = first;  //берем первый сегмент
    int n = 0;
    if (id>segmentCount || id < 0)  //если этого сегмента не существует, но возвращаем пустую сылку
        return nullptr;
    while (n!= id) //пока не дошли до сегмента
    {
        i=i->next; //берем сегмент
        n++; //идем дальше
    }
    return i;  //возвращаем нужный сегмент
}

void List::CopyElement(void* destination, void* source)  //копируем элемент
{
    for (int i = 0; i < elementSize; i++)
        *((char*)destination + i) = *((char*)source + i);
}

void* List::Add(void* data)   //добавляем элемент в список, на вход получаем дату, которую хотим дабавить
{
    if (data==nullptr)  //если дата пуста, то возвращаемпустую ссылку
        return nullptr;
    int segmentNumber = lastIndex/elementCount;  //получаем номер последнего сегмента
    int cell = lastIndex % elementCount;  //получаем количество элементов в последнем сегменте
    Segment* segment = (cell==0) ? NewSegment() : GetSegment(segmentNumber);  //если последний сегмент занят, то задаем новый сегмент, в противном случае
                                                                              //получаем последний сегмент
    char* offset = (char*)segment->data + (cell*elementSize);  //получаем смещение для элемента, который хотим добавить
    CopyElement(offset, data); //копируем элемент
    lastIndex++;  //увеличиваем номер последнего элемента
    return offset;  //возвращаем смещение
}

void List::TakeFirst(void* store)  //Извлечение первого
{
	if (Count() == 0) //если возвращать нечего
	{
		store = nullptr;
		return;
	}
	char* source = (char*)first->data + firstIndex*elementSize;  //находим место элемента в хипе
	CopyElement(store, source); //копируем элемент в стор
	firstIndex++;  //увеличиваем индекс первого элемента

	if (firstIndex == elementCount)  //если номер первого элемента равен количеству элементов в сегменте (стоит в сегменте последним)
	{
		firstIndex -= elementCount;
		lastIndex -= elementCount;
		DeleteSegment(first);
	}
}

void List::TakeLast(void* store)
{
	if (Count() == 0)  //если возвращать нечего
	{
		store = nullptr;
		return;
	}
	char* source = (char*)last->data + ((lastIndex-1) % elementCount)*elementSize;  //находим место элемента в хипе
	CopyElement(store, source);  //копируем элемент в стор
	lastIndex--;  //уменьшаем индекс последнего элемента

	if (lastIndex % elementCount == 0)  //если последним элементом стал последний элемент в сегменте, то удаляем последний сегмент(он теперь пустой)
	{
		DeleteSegment(last);
	}
}

void List::Take(int pos, void* store)
{
	if (Count() == 0)  //если элементов нет, возвращаем нулевую ссылку
	{
		store = nullptr;
		return;
	}
	if (pos == 0)  //если элемент первый
	{
		TakeFirst(store);
		return;
	}
	if (pos == firstIndex + lastIndex-1) //если элемент последний
	{
		TakeLast(store);
		return;
	}

	pos += firstIndex; //определяем позицию нужного элемента
	Segment* currentSegment = GetSegment(pos / elementCount);  //выбираем сегмент, в котором хранится наш элемент
	int elementIndex = pos % elementCount;  //находем индекс желаемого элемента

	char* source = (char*)currentSegment->data + elementIndex*elementSize; //получаем место элемента в хипе
	CopyElement(store, source);  //копируем элемент

	char* destination;

	while (pos != lastIndex)  //пока элемент не последний
	{
		destination = (char*)currentSegment->data + elementIndex*elementSize;  //получаем место элемента в хипе

		if (elementIndex < elementCount - 1)  //если индекс нашего элемента меньше чем количество элементов в сегменте
			source = (char*)currentSegment->data + (elementIndex + 1)*elementSize;
		else
			source = (char*)currentSegment->next->data;

		CopyElement(destination, source);  //сдвигаем элементы
		elementIndex++; //переходим на следующий элемент
		pos++;  //переходим на следующую позицию

		if (elementIndex == elementCount)    //если это последний элемент в сегменте
		{
			currentSegment = currentSegment->next;  //тогда переходим в следующий сегмент
			elementIndex = 0;   //и в нём уже начинаем идти с начала
		}
	}

	if (IsFree(currentSegment)) DeleteSegment(currentSegment);  //если наш сегмент освободился, то удаляем его


	lastIndex--;  //уменьшаем инфекс последнего элемента
}

bool List::IsFree(Segment* segment)   //проверяем, свободен ли сегмент
{
	Segment* i = first; //берём певый сегмент
	int j = 0;
	while (i)  //пока проходим по сегментам
	{
		j++;
		i = i->next;  //считаем количество сегментов
	}

	if (lastIndex /*+ firstIndex*/ <= (j-1)*elementCount)  //если номер поседнего элемента меньше или равен количеству элементов в предыдущих сегментах
		return true; //то говорим, что свооден

	return false;  //в противном случае занят
}

void List::Sort(bool dir = true)
{
    int n = Count();
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if (dir)
			{
				if (*(double*)GetAccessor(j) > *(double*)GetAccessor(j + 1)) Swap(j, j + 1);
			}
			else
			{
				if (*(double*)GetAccessor(j) < *(double*)GetAccessor(j + 1)) Swap(j, j + 1);
			}
		}
	}
}

int List::Count()
{
	return (lastIndex - firstIndex);
}

bool List::Error()
{
	return error;
}

List::Segment* List::NewSegment()  //выделяем новый сегмент
{
	Segment* temp = new Segment(); //инициализируем временный сегмент

	if (first == nullptr) //если первого сегмента не существует
	{
		first = last = temp; //временный делаем первым и последним
		first->next = first->prev = nullptr;  //обнуляем ссылки на предыдущий у первого и на следующий у последнего
	}

	else //если существует первый сегмент
	{
		last->next = temp;  //добавляем сегмент в конец, меняем ссылки
		temp->prev = last;
		last = temp;
	}

	try
	{
		temp->data = Heap::Instance().GetMemory(elementCount*elementSize);
	}
	catch (bad_alloc)
	{
		error = true;
		return nullptr;
	}

	segmentCount++;  //увеличиваем количемство сегментов
	return temp;  //возвращаем новый сегмент
}

void List::DeleteSegment(Segment* segment)  //удаляем сегмент
{
	if (segment == first) first = first->next;   //если это первый сегмент, то первым делаем следующий за ним
	if (segment == last) last = last->prev;  //если это последний сегмент, то последним делаем предпоследний

	if (segment->prev) segment->prev->next = segment->next;  //если существует предыдущий, то меняем ссылки
	if (segment->next) segment->next->prev = segment->prev;  //если существует следующий сегмент, то меняем ссылки
	Heap::Instance().FreeMemory(segment->data);  //освобождаем этого сегмента в хипе
	segmentCount--;  //уменьшаем количество сегментов
}

void* List::GetAccessor(int n)
{
	n += firstIndex;   //получаем номер элемента

	if (n >= lastIndex)  //если элемент за пределами листа, выкидываем исключение
		throw Exceptions::ArgumentOutOfRange;

	Segment* currentSegment = GetSegment(n / elementCount);  //получаем сегмент, в котором находится наш элемент
	int elementIndex = n % elementCount;  //получаем индекс нашего элемента в каррент сегменте

	char* source = (char*)currentSegment->data + elementIndex*elementSize;  //получаем этот элемент
	return source;  //возвращаем этот элемент
}

void List::SetAccessor(void* data, int n)
{
	n += firstIndex;  //получаем номер, куда хотим поместить
	Segment* currentSegment = GetSegment(n / elementCount); //получаем сегмент, в который хотим поместить
	int elementIndex = n % elementCount;  //получаем индекс в сегменте

	char* source = (char*)currentSegment->data + elementIndex*elementSize;  //получаем место в хипе, куда хотим положить
	CopyElement(source, data); //копируем элемент в соурс
}

void List::Swap(int a, int b)  //своп двух элементов
{
	void* temp = Heap::Instance().GetMemory(elementSize);  //выделяем временный кусок памяти, чтобы осуществить своп
	CopyElement(temp, GetAccessor(a));  //копируем первый элемент во временный кусок памяти
	SetAccessor(GetAccessor(b), a);  //кладём второй элемент на место первого
	SetAccessor(temp, b);  //кладём первый из временного на место второго
}

