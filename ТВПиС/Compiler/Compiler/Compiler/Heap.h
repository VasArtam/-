#pragma once

#define SEGMENTSIZE 65539
#define DESCRIPTORCOUNT 1024

class Heap
{
public:
	Heap(int _segment_size = SEGMENTSIZE);
	~Heap();

	//���������� offset ���������� �����
	void* get_mem(int size);

	//����������� ������ �� ������ �����
	void free_mem(void*);

	//�����, ����� ��������� Heap ������ ��� ����
	static Heap& Instance(int segmentSize = SEGMENTSIZE);

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

		//��������� �� ���������� �������
		Segment* prev;

		//������ ������������
		Segment_def descriptor[DESCRIPTORCOUNT];

		//������� ���������� ������������ (��� ID: auto-increment � ������ �������������)
		int descriptor_count;
	};

	//������� ����� �������, ������������� current �� ���� �������
	void make_segment();

	//������� ��� ����� ����� �������� �� ���� ������� �����, ������� ��������� �� ���
	void split_descriptor(Segment* s, int position, int size);

	//������� ��� ��������
	void delete_segments();

	//���������� ������� ��������, ������ ���������� ���������� ����������
	int segment_size;

	//������� �������
	Segment* current;
};
