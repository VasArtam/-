#include "heap.h"
#include <stdlib.h>

using namespace std;

struct Segment;

void*   Heap::get_mem(int size)
{
    //����, ���� �� � ���������� ��������� ���������� ������ ��� size
    //����� �������� � �������� �������: �� �������� �� �������
    Segment* s = current;
    Segment_def found;

    while (s != nullptr)
    {
        int memory = 0;

        //���������� ������, ���������� ����� ������� � ������� ��������
        for (int i = 0; i < s->descriptor_count - 1; i++)
        {
            if (s->descriptor[i].used)
            {
                memory += s->descriptor[i].size;
            }
        }

        //���� ������������� ������ + ������� ������ + ������,
        //��������� ��� ����������� ������, ��� ������,
        //���������� ������� ���������, ��:
        if (size + memory + sizeof(Segment_def)*1024 < sizeof(t))
        {
            //���� ������ ��������� ����
            for (int i = 0; i < s->descriptor_count - 1; i++)
            {
                if (s->descriptor[i].used == false)
                {
                    //------------����������--(descriptor_count)------------//
                    if (s->descriptor[i].size);
                    //���� found ��������� �� ���� ����
                    found = s->descriptor[i];

                    //���������� ��������� ����� �����
                    found.used = true; //�������
                    found.size = size; //������

                    //��������� �� ��������� ����, ���� �� ����
                    if (i < s->descriptor_count - 2)
                    {
                        found.offset = &s->descriptor[i+1];
                    }
                    //���� ������ ���, �� ���� ��������� �� ����� ������ ����
                    else
                    {
                        found.offset = &s->descriptor[0];
                    }
                    return &found;
                }
            }
        }
        //���� �� � ���� �������� ��� ��������� ������,
        //�� �������� ��� �� �� ����� ��� ����������� ��������
        s = s->prev;
    }

    s = make_segment();

    found = s->descriptor[0];
    found.used = true; //�������
    found.size = size; //������
    found.offset = &s->descriptor[1];
    return &found;
};

void    Heap::free_mem (void* offset)
{
    //���� �����, ��� ����� ���������� ������������
    //����� �������� � �������� �������: �� �������� �� �������
    Segment* s = current;

    while (s != nullptr)
    {
        //���� ����� ��������� ����� ������� ������ �������� �
        //������� ��� �����, �� ��������� ����� � ���� ��������,
        if (s->descriptor[0].offset <= offset &&
            (char*)s->descriptor[0].offset + sizeof(t) > offset)
        {
            //�������� �� ���� ������������ ������ ��������
            int i = 0;
            while (i < s->descriptor_count - 1)
            {
                //���� �� ����� ������ �����, ���������� ������������
                if (s->descriptor[i].offset == offset)
                {
                    //���������� ��� ��� ��������������
                    s->descriptor[i].used = false;

                    //���� ��������� ���� - �� ���������, ��
                    //��������� ��� ������ ����� � ����,
                    //� �����
                    if (i >= 1 && s->descriptor[i-1].used == false)
                    {
                        //
                        s->descriptor[i-1].size += s->descriptor[i].size;
                    }

                    /*
                    //���� � �������� ������ �� �������� ������� ������
                    //� ��� �� ��������� ���������� �������,
                    //�� ������ ���� �������.

                    if (s.IsEmpty() && s.prev != nullptr)
                    {
                        s.ClearSegment(); // - �� ���� ������
                    }
                    */

                    return;
                }
                //���� ��� - �� ���� � ���������� ������
                i++;
            }
        }
        //���� ������ ���, �� ���� � ����������� ��������
        s = s->prev;
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
