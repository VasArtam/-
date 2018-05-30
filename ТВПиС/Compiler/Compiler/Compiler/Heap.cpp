#include <stdlib.h>
#include "heap.h"

using namespace std;

void* Heap::get_mem(int size)
{
	//����, ���� �� � ���������� ��������� ���������� ������ ��� size
	//����� �������� � �������� �������: �� �������� �� �������
	Segment* s = current;

	while (s != nullptr)
	{
		int descriptor_count = s->descriptor_count;
		//������� ���� ���������� � ����� �������� ������, ������� �����
		for (int i = 0; i < descriptor_count; i++)
		{
			Segment_def* descriptor = &s->descriptor[i];
			if (!descriptor->used && descriptor->size == size)
			{
				descriptor->used = true; //�������� ��� �������
				return descriptor->offset; //���������� ��������� �� ������� ������
			}
		}

		//���� ���� � �������� ������ ��� ���������, ���� � �����, ����� ������� ������ ������������
		for (int i = descriptor_count - 1; i >= 0; i--)
		{
			Segment_def* descriptor = &s->descriptor[i];
			//���� ����� ��������� ���� � ��� ������ ������ �������������� �������
			if (!descriptor->used && descriptor->size > size)
			{
				//���� ����� �� ����������� ������ ���, �� ������ ����
				if (descriptor_count == DESCRIPTORCOUNT)
				{
					descriptor->used = true; //�������� ��� �������
				}
				else
				{
					split_descriptor(s, i, size);
				}
				
				return descriptor->offset; //���������� ��������� �� ������� ������
			}
		}

		//���� �� � ���� �������� ��� ��������� ������,
		//�� �������� ��� �� �� ����� ��� ����������� ��������
		s = s->prev;
	}

	//���� �� �� � ����� �������� ��� ��������� ������,
	//�� �������� �����
	make_segment();

	split_descriptor(current, 0, size);

	return current->descriptor[0].offset;
};

void Heap::split_descriptor(Heap::Segment* s, int position, int size)
{
	Segment_def* descriptor = &s->descriptor[position];
	//������� ��� ����������� ����, ������� ���� ���������, ������ ����� ��� ������
	for (int j = s->descriptor_count; j > position + 1; ++j)
	{
		s->descriptor[j] = s->descriptor[j - 1];
	}

	Segment_def* next_descriptor = &s->descriptor[position + 1];

	next_descriptor->size = descriptor->size - size;
	next_descriptor->offset = (void*)(int(descriptor->offset) + size);
	next_descriptor->used = false;

	descriptor->size = size;
	descriptor->used = true;

	//����������� ���������� ������������
	++s->descriptor_count;
}


void Heap::free_mem(void* offset)
{
	//���� �����, ��� ����� ���������� ������������
	//����� �������� � �������� �������: �� �������� �� �������
	Segment* s = current;

	while (s != nullptr)
	{
		//���� ����� ��������� ����� ������� ������ �������� � ������� ��� �����, �� ��������� ����� � ���� ��������
		if (s > offset || s + segment_size <= offset)
		{
			//���� ������ ���, �� ���� � ����������� ��������
			s = s->prev;
		}

		int descriptor_count = s->descriptor_count;
		//���� � ������������ ����� �����
		//�����: ���� ����� ����� �������� �� ��� ����������, ������ �� ����������
		for (int i = 0; i < descriptor_count; ++i)
		{
			Segment_def* descriptor = &s->descriptor[i];
			if (descriptor->offset != offset) continue;

			//���� �� ����� ������ �����, ���������� ������������
			descriptor->used = false;

			int movefrom = i;
			int moveto = i;
			Segment_def* prev_descriptor = &s->descriptor[i - 1];

			//��������� �������� �����, ���� �����-�� �� ��� ������
			if (i != 0 && prev_descriptor->used == false)
			{
				prev_descriptor->size += descriptor->size;
				++movefrom;
			}

			Segment_def* next_descriptor = &s->descriptor[i + 1];
			if (i != descriptor_count - 1 && next_descriptor->used == false)
			{
				if (movefrom != i)
				{
					prev_descriptor->size += next_descriptor->size;
					++movefrom;
				}
				else
				{
					descriptor->size += next_descriptor->size;
					descriptor->used = false;
					movefrom += 2;
					++moveto;
				}
			}

			//������� �����������
			if (moveto != movefrom)
			{
				while (movefrom < descriptor_count)
				{
					s->descriptor[moveto++] = s->descriptor[movefrom++];
				}

				s->descriptor_count -= movefrom - moveto;
			}


			//���� � �������� ������ �� �������� ������� ������
			//� ��� �� ��������� ���������� �������,
			//�� ������ ���� �������.

			bool empty = true;

			for (int j = 0; j < s->descriptor_count - 1; j++)
			{
				if (s->descriptor[j].used)
				{
					empty = false;
					break;
				}
			}

			if (empty)
			{
				current = s->prev;
				free(s);
			}

			return;
		}
	}
};

void Heap::make_segment()
{
	Segment* temp = (Segment*)malloc(segment_size); //�������� ������ ��� ����� �������
	const unsigned descriptors_size = sizeof(Segment_def) * 1024;
	temp->data = temp + descriptors_size; //��������� �� ����� � �������
	temp->prev = current; //���������� ������� - �������

	Segment_def* first_descriptor = &temp->descriptor[0];
	//������ ��������� ������� �����
	first_descriptor->used = false;
	first_descriptor->size = segment_size - descriptors_size; //������ ����� �������� ����� ������ ������� ������������
	first_descriptor->offset = temp->data; //��������� �� ����� �������

	temp->descriptor_count = 1; //���������� ������ - 1

	current = temp; //������� ���������� �����
};

void Heap::delete_segments()
{
	while (true)
	{
		Segment* s = current;

		//�������� ������ � ������������
		for (int i = 0; i < s->descriptor_count; i++)
		{
			s->descriptor[i].used = false;
			s->descriptor[i].size = 0;
			s->descriptor[i].offset = nullptr;
		}

		//������� - ���������
		if (current->prev == nullptr)
		{
			free(current);
			make_segment(); //�������� ����� ������
			return;
		}

		current = current->prev;
		free(s);
	}
};