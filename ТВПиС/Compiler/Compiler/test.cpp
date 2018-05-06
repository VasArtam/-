#include <stdlib.h>
#include <cstdio>

int main()
{
    /*Heap heap = new Heap();

    char* arr = heap.get_mem(10*sizeof(char));*/
    char *arr;
    arr = (char*)malloc(10*sizeof(char));

    char **ptr_arr = &arr;

    cout << arr;
    cout << &arr;
}
