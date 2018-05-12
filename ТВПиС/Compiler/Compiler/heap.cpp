#include "heap.h"
#include <stdlib.h>

using namespace std;

struct Segment;

void*   Heap::get_mem(int size)
{
    //����, ���� �� � ���������� ��������� ���������� ������ ��� size
    //����� �������� � �������� �������: �� �������� �� �������
    Segment* t = current;
    Segment_def found;

    while (t != nullptr)
    {
        int memory = 0;

        //���������� ������, ���������� ����� ������� � ������� ��������
        for (int i = 0; i < t->descriptor_count - 1; i++)
        {
            if (t->descriptor[i].used)
            {
                memory += t->descriptor[i].size;
            }
        }

        //���� ������������� ������ + ������� ������ + ������,
        //��������� ��� ����������� ������, ��� ������,
        //���������� ������� ���������, ��:
        if (size + memory + sizeof(Segment_def)*1024 < sizeof(t))
        {
            //���� ������ ��������� ����
            for (int i = 0; i < t->descriptor_count - 1; i++)
            {
                if (t->descriptor[i].used == false)
                {
                    //------------����������--(descriptor_count)------------//
                    if (t->descriptor[i].size);
                    //���� found ��������� �� ���� ����
                    found = t->descriptor[i];

                    //���������� ��������� ����� �����
                    found.used = true; //�������
                    found.size = size; //������

                    //��������� �� ��������� ����, ���� �� ����
                    if (i < t->descriptor_count - 2)
                    {
                        found.offset = &t->descriptor[i+1];
                    }
                    //���� ������ ���, �� ���� ��������� �� ����� ������ ����
                    else
                    {
                        found.offset = &t->descriptor[0];
                    }
                    return &found;
                }
            }
        }
        //���� �� � ���� �������� ��� ��������� ������,
        //�� �������� ��� �� �� ����� ��� ����������� ��������
        t = t->prev;
    }

    t = make_segment();

    found = t->descriptor[0];
    found.used = true; //�������
    found.size = size; //������
    found.offset = &t->descriptor[1];
    return &found;
};

void    Heap::free_mem (void* offset)
{
    //���� �����, ��� ����� ���������� ������������
    //����� �������� � �������� �������: �� �������� �� �������
    Segment* t = current;

    while (t != nullptr)
    {
        //���� ����� ��������� ����� ������� ������ �������� �
        //������� ��� �����, �� ��������� ����� � ���� ��������,
        if (t->descriptor[0].offset <= offset &&
            (char*)t->descriptor[0].offset + sizeof(t) > offset)
        {
            //�������� �� ���� ������������ ������ ��������
            int i = 0;
            while (i < t->descriptor_count - 1)
            {
                //���� �� ����� ������ �����, ���������� ������������
                if (t->descriptor[i].offset == offset)
                {
                    //���������� ��� ��� ��������������
                    t->descriptor[i].used = false;

                    //���� ��������� ���� - �� ���������, ��
                    //��������� ��� ������ ����� � ����,
                    //� �����
                    if (i >= 1 && t->descriptor[i-1].used == false)
                    {
                        //
                        t->descriptor[i-1].size += t->descriptor[i].size;
                    }

                    /*
                    //���� � �������� ������ �� �������� ������� ������
                    //� ��� �� ��������� ���������� �������,
                    //�� ������ ���� �������.

                    if (t.IsEmpty() && t.prev != nullptr)
                    {
                        t.ClearSegment(); // - �� ���� ������
                    }
                    */

                    return;
                }
                //���� ��� - �� ���� � ���������� ������
                i++;
            }
        }
        //���� ������ ���, �� ���� � ����������� ��������
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
