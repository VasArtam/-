#define SEGMENTSIZE 65539
#define SEGMENTCOUNT 1024

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

        void*   get_mem(int size)
        {
            int memory = SEGMENTCOUNT*sizeof(Segment_def);
            int i = 0;
            while (i <= current->descriptor_count)
            {
                memory += current->descriptor[i].size;
                if (memory == segment_size)
                {
                    make_segment();
                    memory = 0;
                    break;
                }
                i++;
            }

            if (memory + size <= segment_size)
            {
                current->descriptor[descriptor_count]->used = true;
                current->descriptor[descriptor_count]->size = size;
                current->descriptor_count++;
                current->descriptor[descriptor_count]->offset = current->descriptor[descriptor_count];
            }



            /*if (current.descriptor[descriptor_count].used == false)
            {

            }
            else
            {
                make_segment();
            }*/
        };

        void    free_mem (void*)
        {

        };

private:
        struct  Segment_def
        {
            bool      used;
            int       size;
            void*     offset;
        };

        struct  Segment
        {
            void*        data;
            Segment*     prev;
            Segment_def  descriptor[SEGMENTCOUNT];
            int          descriptor_count;
        };

        Segment()
        {
            data = malloc(segment_size*sizeof(char));
            prev = current;
            current = prev;
        };

        int     make_segment()
        {
            /*Segment* segment = (Segment*)malloc(sizeof(Segment));
            segment.data = (char*) malloc(segment_size);
            segment.prev = current;
            segment.descriptor_count = 0;
            current = *segment;
            return 1;*/
            Segment* temp = new Segment(segmentSize);
            current = temp;
            return 1;
        };

        void    delete_segments()
        {
            Segment* temp;
            while (current != 0)
            {
                if (current->prev == 0 || current->prev == nullptr)
                {
                    //temp = current.prev;
                    free(current->data);
                    &temp = nullptr;
                }
                else
                {
                    temp = current->prev;
                    &current = nullptr;
                }
                &current = nullptr;
                current = temp;
            }
        };

        int     segment_size;

        Segment*    current;
};
