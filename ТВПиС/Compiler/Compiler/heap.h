#pragma once

#define SEGMENTSIZE 65539
#define DESCRIPTORCOUNT 1024

class Heap
{
public:
        Heap(int _segment_size = SEGMENTSIZE)
        {
            segment_size  = _segment_size;
            current       = 0;
        };
        ~Heap()
        {
            delete_segments();
        };

        void*   get_mem(int size);

        void    free_mem (void*);

private:
        struct  Segment_def
        {
            bool      used;
            int       size;
            void*     offset;
        };

        struct  Segment
        {
            Segment() { Segment(segment_size); };
            Segment(int _segment_size);
            void*        data;
            Segment*     prev;
            Segment_def  descriptor[DESCRIPTORCOUNT];
            int          descriptor_count;
        };

        Segment*    make_segment();

        void    delete_segments();

        int     segment_size;

        Segment*    current;
};
