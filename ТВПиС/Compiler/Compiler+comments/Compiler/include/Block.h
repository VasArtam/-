#ifndef BLOCK_H
#define BLOCK_H

struct Block
{
    bool used;
    int size;
    void* offset;

    Block* next;
    Block* prev;
};

#endif // BLOCK_H
