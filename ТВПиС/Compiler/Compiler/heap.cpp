#include "heap.h"
#include <stdlib.h>

using namespace std;

struct Segment;

void*   Heap::get_mem(int size)
{
    //Èùåì, åñòü ëè â ïðåäûäóùèõ ñåãìåíòàõ äîñòàòî÷íî ïàìÿòè äëÿ size
    //Ïîèñê íà÷èíàåì â îáðàòíîì ïîðÿäêå: îò òåêóùåãî äî ïåðâîãî
    Segment* t = current;
    Segment_def found;

    while (t != nullptr)
    {
        int memory = 0;

        //Ïîäñ÷èòàåì ïàìÿòü, çàíèìàåìóþ âñåìè áëîêàìè â òåêóùåì ñåãìåíòå
        for (int i = 0; i < t->descriptor_count - 1; i++)
        {
            if (t->descriptor[i].used)
            {
                memory += t->descriptor[i].size;
            }
        }

        //Åñëè çàïðàøèâàåìàÿ ïàìÿòü + çàíÿòàÿ ïàìÿòü + ïàìÿòü,
        //îòâîäèìàÿ ïîä äåñêðèïòîðû ìåíüøå, ÷åì ïàìÿòü,
        //çàíèìàåìàÿ òåêóùèì ñåãìåíòîì, òî:
        if (size + memory + sizeof(Segment_def)*1024 < sizeof(t))
        {
            //Èùåì ïåðâûé íåçàíÿòûé áëîê
            for (int i = 0; i < t->descriptor_count - 1; i++)
            {
                if (t->descriptor[i].used == false)
                {
                    //------------ÏÅÐÅÏÈÑÀÒÜ--(descriptor_count)------------//
                    if (t->descriptor[i].size);
                    //Äàåì found óêàçàòåëü íà ýòîò áëîê
                    found = t->descriptor[i];

                    //Îïðåäåëÿåì ïàðàìåòðû ýòîãî áëîêà
                    found.used = true; //çàíÿòûé
                    found.size = size; //ðàçìåð

                    //Óêàçàòåëü íà ñëåäóþùèé áëîê, åñëè îí åñòü
                    if (i < t->descriptor_count - 2)
                    {
                        found.offset = &t->descriptor[i+1];
                    }
                    //Åñëè òàêîãî íåò, òî äàåì óêàçàòåëü íà ñàìûé ïåðâûé áëîê
                    else
                    {
                        found.offset = &t->descriptor[0];
                    }
                    return &found;
                }
            }
        }
        //Åñëè æå â ýòîì ñåãìåíòå íåò ñâîáîäíîé ïàìÿòè,
        //òî ïîâòîðèì âñå òî æå ñàìîå äëÿ ïðåäûäóùåãî ñåãìåíòà
        t = t->prev;
    }

    t = make_segment();

    found = t->descriptor[0];
    found.used = true; //çàíÿòûé
    found.size = size; //ðàçìåð
    found.offset = &t->descriptor[1];
    return &found;
};

void    Heap::free_mem (void* offset)
{
    //Ищем адрес, где нужно произвести освобождение
    //Поиск начинаем в обратном порядке: от текущего до первого
    Segment* t = current;

    while (t != nullptr)
    {
        //Если адрес находится между адресом начала сегмента и
        //адресом его конца, то выполняем поиск в этом сегменте,
        if (t->descriptor[0].offset <= offset &&
            (char*)t->descriptor[0].offset + sizeof(t) > offset)
        {
            //Проходим по всем дескрипторам нашего сегмента
            int i = 0;
            while (i < t->descriptor_count - 1)
            {
                //Если мы нашли нужный адрес, производим освобождение
                if (t->descriptor[i].offset == offset)
                {
                    //Обозначаем его как неиспользуемый
                    t->descriptor[i].used = false;

                    //Если найденный блок - не последний, то
                    //превратим два пустых блока в один,
                    //а адрес
                    if (i >= 1 && t->descriptor[i-1].used == false)
                    {
                        //
                        t->descriptor[i-1].size += t->descriptor[i].size;
                    }

                    /*
                    //Если в сегменте больше не осталось занятых блоков
                    //и это не последний оставшийся сегмент,
                    //то удалим этот сегмент.

                    if (t.IsEmpty() && t.prev != nullptr)
                    {
                        t.ClearSegment(); // - не есть хорошо
                    }
                    */

                    return;
                }
                //Если нет - то идем к следующему адресу
                i++;
            }
        }
        //Если такого нет, то идем к предыдущему сегменту
        t = t->prev;
    }
};

Heap::Segment*     Heap::make_segment()
{
    /*Segment* segment = (Segment*)malloc(sizeof(Segment));
    segment.data = (char*)malloc(segment_size);
    segment.prev = current;
    segment.descriptor_count = 0;
    current = *segment;
    return 1;*/
    Segment* temp = new Segment();
    current = temp;
    return current;
};

void    Heap::delete_segments()
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
