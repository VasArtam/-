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

	void* get_mem(int size); //возвращает offset найденного блока

	void free_mem(void*); //освобождает пам€ть по адресу блока

private:
	struct Segment_def
	{
		bool used; //используетс€ ли блок (1 байт)
		int size; //размер блока (4 байта)
		void* offset; //указатель на данные (неизвестно сколько байт)
	};

	struct Segment
	{
		void* data;
		Segment* prev; //указатель на предыдущий сегмент
		Segment_def descriptor[DESCRIPTORCOUNT]; //массив дескрипторов
		int descriptor_count; //текущее количество дескрипторов (как ID: auto-increment и только увеличиваетс€)
		//последний дескриптор сегмента - тот, от которого откусывали новые блоки
	};

	void make_segment(); //создает новый сегмент, устанавливает current на этот сегмент

	void delete_segments(); //удал€ет все сегменты

	int segment_size; //переменна€ размера сегмента, просто выт€гивает глобальную переменную

	Segment* current; //текущий сегмент
};
