#include "Segment.h"

Segment::Segment(Segment* prev, int segmentSize) //конструктор, принимает на вход предыдущий сегмент и размер сегмента
{
    this->prev = prev;    //закладывается значение предыдущего сегмента
    firstBlock = lastBlock = new Block;    //первому и последему блоку присваивается новый блок
    if(!(firstBlock->offset = data = malloc(segmenetSize)))  //
    {
        throw new bad_alloc();
    }
    firstBlock->used = false;   //первый блок не использован
    firstBlock->size = this->segmentSize = segmentSize; //в размер первого блока и размер сегмента закладываем принятый на вход размер сегмента
    firstBlock->prev = firstBlock->next = nullptr;  //обнуояем ссылки на предыдущий и следующий блок у первого блока
}

Segment::~Segment(void)  //деструктор семгмента
{
    ClearSegment();
}

void* Segment::PushBlock(int blockSize)
{
    if (blockSize > segmentSize)  //если размер блока больше чем размер сегмента выбрасываем исключение
    {
        throw new bad_alloc();
    }
    Block* i = firstBlock; //берём первый блок в сегменте
    while(i)  //пока проходим по блокам
    {
        if (!i->used)  //если блок не использован
        {
            if(i->size > blockSize) //если размер этого блока больше, чем блок который мы хотим туда поместить
            {
                Block* newBlock = new Block;  //создаем новый блок
                newBlock->used = true;     //помечаем его как используемый
                newBlock->size = blockSize;   //закладываем в него размер блока
                newBlock->offset = i->offset;  //закладываем смещение
                i->offset = (char*)newBlock->offset + newBlock->size; //в смещение блока i закладываем (смещение нового блока плюс его размер)
                i->size -= blockSize; //уменьшаем размер блока i
                if (newBlock->prev = i->prev)  //если prev нового блока удалось присвоить prev блока i
                {
                    newBlock->prev->next = newBlock; //тогда предшествующему в next закладываем новый блок
                }
                i->prev = newBlock; //для блока i в предыдущий закладываем новый блок
                newBlock->next = i;  //в следующий блок нового закладываем i
                if(i == firstBlock)  //если i это первый блок
                    firstBlock = newBlock;  //тогда первому блоку(для сегмента) присваиваем новый блок
                return newBlock->offset;  //возвращаем смещение нового блока
            }
            else if (i->size == blockSize) //если размер этого блока равен размеру блока который хотим в него поместить, то просто впихиваем его туда
            {

                i->used = true;    //помечаем его как используемый
                return i->offset;  //возвращаем смещение этого блока
            }
        }
        i = i->next;  //переходим на следующий блок
    }
    return nullptr;  //если впихнуть блок не удалось, то возвращаем нулевую ссылку
}

void Segment::RemoveBlock(void* oldBlock)  //удаляем блок (получая ссылку на блок)
{
	Segment* i = this; //выбираем этот сегмент
	while (i) //пока идём по сегментам
	{
		if (i->firstBlock->offset <= oldBlock && (char*)i->firstBlock->offset + SEGMENTSIZE > oldBlock)  //если нужный блок находится в этом сегменте
		{
			Block* j = i->firstBlock;  //берем первый блок из этого сегмента
			while (j)   //пока идем по блокам сегмента
			{
				if (j->offset == oldBlock)  //если найден нужный сегмент, то вызываем удаление блока
				{
					RemoveBlock(j);

					//Если в странице не осталось больше занятых блоков и это не последняя страница, то удаляем ее
					/*if (i->IsEmpty() && i->prev)
					{
						i->ClearSegment(); //Не хорошо.
					}*/

					return;
				}
				j = j->next; //переходим к следующему блоку
			}
		}

		i = i->prev;  //переходим к следующему сегменту
	}
}

void Segment::RemoveBlock(Block* oldBlock)  //удаляем блок (получая блок)
{
	oldBlock->used = false; //помечаем, что блок не исплользуется

	if (oldBlock->prev && !oldBlock->prev->used) //если у блока есть предыдущий и предыдущий не используется
	{
		if (oldBlock->prev == firstBlock)  //если предыдущий блок является первым, то в первый блок запихиваем и наш удаляемый (расширяем размер
            firstBlock = oldBlock;
		oldBlock->size += oldBlock->prev->size;  //к размеру удаляемого блока добавляем размер предыдущего
		oldBlock->offset = (char*)oldBlock->offset - oldBlock->prev->size; //смещение удаляемого блока уменьшаем на размер предыдущего

		if (oldBlock->prev->prev) //если у предыдущего существует предыдущий, то делаем для него следующим удаляемый блок
		{
			oldBlock->prev->prev->next = oldBlock;
		}
		oldBlock->prev = oldBlock->prev->prev; //смещаем ссылку на предыдущий на блок назад
	}

	if (oldBlock->next && !oldBlock->next->used) //если существует следующий и следующий не используется
	{
		oldBlock->size += oldBlock->next->size; //тогда размер удаляемого увеличиваем на размер следующего за ним

		if (oldBlock->next == lastBlock)  //если следующий блок последний, то последнему в сегменте присваиваем удаляемый
            lastBlock = oldBlock;

		if (oldBlock->next->next)  //если у следующего есть следующий
		{
			oldBlock->next->next->prev = oldBlock;  //смещаем ссылку
		}
		oldBlock->next = oldBlock->next->next;   //смещаем ссылку на блок вперёд
	}
}

void Segment::ClearSegment()   //очищение сегмента полностью
{
	free(this->data);  //освобождаем дату

	this->firstBlock = this->lastBlock = nullptr;    //обнуляем ссылки у первого и последнего блоков в этом сегменте
	this->data = this->prev = nullptr;   //обнуляем дату и предыдущий сегмент у сегмента
	this->segmentSize = 0;   //обнуляем размер сегмента
}

bool Segment::IsEmpty()     //проверяем сегмент на пустоту
{
	if (this->firstBlock == this->lastBlock && this->firstBlock->size == false)   //если первый сегмент это последний сегмент и у первого нет размера
		return true;  //тогда говорим что сегмент пустой
	return false;  //в проивном случае, в сегменте что-то есть
}
