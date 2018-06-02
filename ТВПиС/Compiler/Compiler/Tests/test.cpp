#include "pch.h"
#include "../Compiler/Heap.h"
#include "../Compiler/Heap.cpp"
#include <tchar.h>

TEST(HeapTest, Second_Allocation_Is_Near_First_Allocation)
{
	const auto heap = new Heap();
	const auto memory = heap->get_mem(128);
	const auto memory1 = heap->get_mem(128);
	ASSERT_EQ(128, int(memory1) - int(memory));
}

TEST(HeapTest, Test_Free)
{
	const auto heap = new Heap();
	const auto memory = heap->get_mem(128);
	const auto memory1 = heap->get_mem(128);
	heap->free_mem(memory);
	heap->free_mem(memory1);
}

TEST(HeapTest, Memory)
{
	int integer = 10;
	void* p = &integer;

	p = static_cast<char*>(p) + 1;

	p = &integer;
	p = static_cast<int*>(p) + 1;

}
