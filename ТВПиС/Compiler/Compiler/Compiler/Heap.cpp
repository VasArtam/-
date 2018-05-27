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
		int memory = 0;

		//���������� ��������� ������
		for (int i = 0; i < s->descriptor_count; i++)
		{
			if (!s->descriptor[i].used)
			{
				memory += s->descriptor[i].size;
			}
		}

		//���� ��������� ������ ������, ��� ��������� (�� ����, ��� �������� �����)
		if (memory >= size)
		{
			//���� ������ ��������� ����
			for (int i = 0; i < s->descriptor_count; i++)
			{
				//���� ����� ��������� ���� � ��� ������ ����� �������������� �������
				if (!s->descriptor[i].used && s->descriptor[i].size == size)
				{
					s->descriptor[i].used = true; //�������� ��� �������
					return s->descriptor[i].offset; //���������� ��������� �� ����
				}
				//���� ����� ��������� ���� � ��� ������ ������ �������������� �������
				else if (!s->descriptor[i].used && s->descriptor[i].size > size)
				{
					//������� ���� = ����, ������� �����
					//��������� ���� = ����� ����

					//��������� ���� �������� ����� �������� �����
					s->descriptor[s->descriptor_count].offset = s->descriptor[i].offset;
					//������ ���������� ����� - ������ ������������� ������
					s->descriptor[s->descriptor_count].size = size;
					//��������� ���� ������������
					s->descriptor[s->descriptor_count].used = true;

					//����������� ���������� ������������
					s->descriptor_count++;

					//������� ���� ���������� ������ �� ������ ������������� ������
					s->descriptor[i].offset = (char*)s->descriptor[i].offset + size;
					//������ �������� ����� ���������� ������
					s->descriptor[i].size -= size;
					//������� ���� �� ������������
					s->descriptor[i].used = false;

					return s->descriptor[s->descriptor_count].offset;
				}
			}
		}
		//���� �� � ���� �������� ��� ��������� ������,
		//�� �������� ��� �� �� ����� ��� ����������� ��������
		s = s->prev;
	}

	//���� �� �� � ����� �������� ��� ��������� ������,
	//�� �������� �����
	make_segment();

	s = current;

	//��������� ���� �������� ����� �������� �����
	s->descriptor[s->descriptor_count].offset = s->descriptor[0].offset;
	//������ ���������� ����� - ������ ������������� ������
	s->descriptor[s->descriptor_count].size = size;
	//��������� ���� ������������
	s->descriptor[s->descriptor_count].used = true;

	//����������� ���������� ������������
	s->descriptor_count++;

	//������� ���� ���������� ������ �� ������ ������������� ������
	s->descriptor[0].offset = (char*)s->descriptor[0].offset + size;
	//������ �������� ����� ���������� ������
	s->descriptor[0].size -= size;
	//������� ���� �� ������������
	s->descriptor[0].used = false;

	return current->descriptor[0].offset;
};

void Heap::free_mem(void* offset)
{
	//���� �����, ��� ����� ���������� ������������
	//����� �������� � �������� �������: �� �������� �� �������
	Segment* s = current;

	while (s != nullptr)
	{
		//���� ����� ��������� ����� ������� ������ �������� � ������� ��� �����, �� ��������� ����� � ���� ��������
		if (s <= offset && s + sizeof(s) > offset)
		{
			//���� � ������������ ����� �����
			//�����: ���� ����� ����� �������� �� ��� ����������, ������ �� ����������
			int i = 0;
			while (i < s->descriptor_count - 1)
			{
				//���� �� ����� ������ �����, ���������� ������������
				if (s->descriptor[i].offset == offset)
				{
					s->descriptor[i].used = false;

					free(s->descriptor[i].offset);

					//��������� �������� �����, ���� �����-�� �� ��� ������
					if (i >= 1 && s->descriptor[i - 1].used == false)
					{
						s->descriptor[i - 1].size += s->descriptor[i].size;
						s->descriptor[i].size = 0;
						s->descriptor[i].offset = s->descriptor[i - 1].offset;
					}
					if (i <= s->descriptor_count - 1 && s->descriptor[i + 1].used == false)
					{
						s->descriptor[i].size += s->descriptor[i + 1].size;
						s->descriptor[i + 1].size = 0;
						s->descriptor[i + 1].offset = s->descriptor[i].offset;
					}

					//���� � �������� ������ �� �������� ������� ������
					//� ��� �� ��������� ���������� �������,
					//�� ������ ���� �������.

					bool empty = true;

					for (int j = 0; i < s->descriptor_count - 1; i++)
					{
						if (s->descriptor[i].used = true) break;
					}

					if (empty)
					{
						//delete_segment(s);
					}

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

void Heap::make_segment()
{
	Segment* temp = (Segment*)malloc(segment_size); //�������� ������ ��� ����� �������
	temp->data = temp + sizeof(Segment_def) * 1024; //��������� �� ����� � �������
	temp->prev = current; //���������� ������� - �������

						  //������ ��������� ������� �����
	temp->descriptor[0].used = false;
	temp->descriptor[0].size = sizeof(temp) - sizeof(Segment_def) * 1024; //������ ����� �������� ����� ������ ������� ������������
	temp->descriptor[0].offset = temp->data; //��������� �� ����� �������

	temp->descriptor_count = 1; //���������� ������������ ������ - 1

	current = temp; //������� ���������� �����
};

void Heap::delete_segments()
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