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
		for (int i = 0; i < n1 * n2 * n3 * n4 * n5; i++)
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
	if (key_word)
		return table[combine_keys(key_word)];

	return nullptr;
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
	Article* article = nullptr;
	article->word = "";
	article->description = "";

	put(article);
}

Diction_list::~Diction_list()
{
	int c = this->count();
	while (c > 0)
	{
		take_last(nullptr);
		c--;
	}
}

void Diction_list::put(Article* article)
{
	this->add(article);
}

Article* Diction_list::find(char* word)
{
	Article* article;

	for (int i = 0; i < this->count(); i++)
	{
		if ((article = static_cast<Article*>(this->get(i)))->word == word)
		{
			return article;
		}
	}

	return nullptr;
}

void Diction_list::del(char* word)
{
	if (word)
	{
		Article* article;
		for (int i = 0; i < this->count(); i++)
		{
			if ((article = static_cast<Article*>(this->get(i)))->word == word)
			{
				take(i, nullptr);
				return;
			}
		}
	}
}

void Diction_list::del(Article* article)
{
	del(article->word);
}

Diction::Diction() : Hash(33, 33, 0, 0, 0)
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
	List* list = nullptr;

	if ((list = find_list(word)) != nullptr)
	{
		Article* article = nullptr;

		for (int i = 0; i < list->count(); i++)
		{
			if ((article = static_cast<Article*>(list->get(i)))->word == word)
			{
				return article;
			}
		}
	}

	return nullptr;
}

Article* Diction::auto_create(char* word)
{
	if (word && find(word) == nullptr)
	{
		Article* article = nullptr;
		article->word = word;
		article->description = "";

		List* list = this->find_list(word);
		list->add(article);

		for (int i = 0; i < list->count(); i++)
		{
			if ((article = static_cast<Article*>(list->get(i)))->word == word)
			{
				return article;
			}
		}
	}
	else return find(word);
	return nullptr;
}
