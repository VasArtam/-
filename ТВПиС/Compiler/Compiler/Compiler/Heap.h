#pragma once

#define SEGMENTSIZE 65539
#define DESCRIPTORCOUNT 1024

class Heap
{
public:
	Heap(int _segment_size = SEGMENTSIZE)
	{
		segment_size = _segment_size;
	};
	~Heap()
	{
		delete_segments();
	};

	void* get_mem(int size); //���������� offset ���������� �����

	void free_mem(void*); //����������� ������ �� ������ �����

private:
	struct Segment_def
	{
		bool used; //������������ �� ���� (1 ����)
		int size; //������ ����� (4 �����)
		void* offset; //��������� �� ������ (���������� ������� ����)
	};

	struct Segment
	{
		void* data;
		Segment* prev; //��������� �� ���������� �������
		Segment_def descriptor[DESCRIPTORCOUNT]; //������ ������������
		int descriptor_count; //������� ���������� ������������ (��� ID: auto-increment � ������ �������������)
		//��������� ���������� �������� - ���, �� �������� ���������� ����� �����
	};

	void make_segment(); //������� ����� �������, ������������� current �� ���� �������

	void delete_segments(); //������� ��� ��������

	int segment_size; //���������� ������� ��������, ������ ���������� ���������� ����������

	Segment* current; //������� �������
};
