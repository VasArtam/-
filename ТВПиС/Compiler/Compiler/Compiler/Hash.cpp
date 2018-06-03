#include "Hash.h"
#include "stdafx.h"
#include <exception>

using namespace std;

Hash::Hash(int _n1, int _n2, int _n3, int _n4, int _n5)
{
	n1 = abs(_n1) % MAXRANGE + 1;
	n2 = abs(_n2) % MAXRANGE + 1;
	n3 = abs(_n3) % MAXRANGE + 1;
	n4 = abs(_n4) % MAXRANGE + 1;
	n5 = abs(_n5) % MAXRANGE + 1;
	table = static_cast<List**>(Heap::Instance().get_mem(n1 * n2 * n3 * n4 * n5 * sizeof(List*)));
	//массив из указателей на листы, выделяем память
}

Hash::~Hash()
{
	if (table)
	{
		int capacity = combine_keys(n1, n2, n3, n4, n5) + 1;
		for (int i = 0; i < capacity; i++)
		{
			if (table[i])
			{
				delete table[i];
				table[i] = nullptr;
			}
		}
		delete[] table;
		table = nullptr;
	}
}

List* Hash::find_list(char* key_word)
{
	return table[combine_keys(key_word)];
}

int Hash::combine_keys(char* key_word)
{
	return
		abs(key5(key_word)) % n5 +
		abs(key4(key_word)) % n4 * n4 +
		abs(key3(key_word)) % n3 * n3 * n4 +
		abs(key2(key_word)) % n2 * n2 * n3 * n4 +
		abs(key1(key_word)) % n1 * n1 * n2 * n3 * n4;
}

Diction_list::Diction_list(): List(sizeof(Article))
{
}

Diction_list::~Diction_list()
{
}

void Diction_list::put(Article * article)
{
}

Article* Diction_list::find(char* word)
{
}

void Diction_list::del(char* word)
{
}

void Diction_list::del(Article* article)
{
	del(article->word);
}

Diction::Diction(): Hash(33, 33, 0, 0, 0)
{
}

Diction::~Diction()
{
}

int Diction::key1(char* key_word)
{
	const int f = key_word[0] - 'A';
	if (f < 33 && f > 0) return f;
	return 0;
}

int Diction::key2(char* key_word)
{
	return key_word[1] % 33;
}

Article* Diction::find(char* word)
{
	Diction_list* list = (Diction_list*)find_list(word);
	return list ? list->find(word) : nullptr;
}

Article* Diction::auto_create(char * word)
{
	return nullptr;
}



List* Hash::FindList(char* keyWord)
{
	if (keyWord)
		return table[CombineKeys(Key1(keyWord), Key2(keyWord), Key3(keyWord), Key4(keyWord), Key5(keyWord))];

	return nullptr;
}

unsigned int Hash::CombineKeys(unsigned int key1, unsigned int key2, unsigned int key3, unsigned int key4, unsigned int key5)
{
	return	n1 ? key1 % (n1 + 1) : 0 +
		n2 ? key2 % (n2 + 1) : 0 +
		n3 ? key3 % (n3 + 1) : 0 +
		n4 ? key4 % (n4 + 1) : 0 +
		n5 ? key5 % (n5 + 1) : 0;
}

