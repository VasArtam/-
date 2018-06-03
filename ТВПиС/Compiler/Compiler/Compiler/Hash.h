#pragma once

#include "Heap.h"
#include "List.h"
#include <cstdlib>

#define MAXRANGE 20

class Hash
{
public:
	Hash(int _n1 = 0, int _n2 = 0, int _n3 = 0, int _n4 = 0, int _n5 = 0); //области значений каждой из пяти функций (таблица 3*3 - 3 3 0 0 0)
	~Hash();

	//самая важная функция, получает ключевое слово и возвращает указатель на отдельный лист
	List* find_list(char* key_word);;

	//переопределить, можно использовать до 5 функций
	virtual int key1(char* key_word) { return 0; };
	virtual int key2(char* key_word) { return 0; };
	virtual int key3(char* key_word) { return 0; };
	virtual int key4(char* key_word) { return 0; };
	virtual int key5(char* key_word) { return 0; };

protected:
	//определяет позицию в векторе из указателей
	int combine_keys(char* key_word);;

private:
	List** table; //сама хэш-таблица
	int    n1, n2, n3, n4, n5; //ее ключи
};

//отдельный элемент в листе
struct Article
{
	char* word;
	char* description;
};

//сам лист
class Diction_list : List
{
public:
	Diction_list();
	~Diction_list(); // purge all Article data

	void     put(Article* article); // allocate memory for word & description
	Article* find(char* word);
	void     del(char* word); // free memory from word & description
	void     del(Article* article);
};

//пример
class Diction : public Hash
{
public:
	Diction();
	~Diction();

	int key1(char* key_word) override;
	int key2(char* key_word) override;

	Article* find(char* word);
	Article* auto_create(char* word); // create Article (and List) if not exist
};
