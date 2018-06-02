#pragma once

#define SEGMENTSIZE 65539
#define DESCRIPTORCOUNT 1024

class Heap
{
public:
	Heap(int _segment_size = SEGMENTSIZE);
	~Heap();

	//Возвращает offset найденного блока
	void* get_mem(int size);

	//Освобождает память по адресу блока
	void free_mem(void*);

	//Нужно, чтобы экземпляр Heap всегда был один
	static Heap& Instance(int segmentSize = SEGMENTSIZE);

private:
	struct Segment_def
	{
		bool used; //используется ли блок (1 байт)
		int size; //размер блока (4 байта)
		void* offset; //указатель на данные (неизвестно сколько байт)
	};

	struct Segment
	{
		void* data;

		//Указатель на предыдущий сегмент
		Segment* prev;

		//Массив дескрипторов
		Segment_def descriptor[DESCRIPTORCOUNT];

		//Текущее количество дескрипторов (как ID: auto-increment и только увеличивается)
		int descriptor_count;
	};

	//Создает новый сегмент, устанавливает current на этот сегмент
	void make_segment();

	//Смещает все блоки после текущего на одну позицию вверх, текущий разделяет на два
	void split_descriptor(Segment* s, int position, int size);

	//Удаляет все сегменты
	void delete_segments();

	//Переменная размера сегмента, просто вытягивает глобальную переменную
	int segment_size;

	//Текущий сегмент
	Segment* current;
};
