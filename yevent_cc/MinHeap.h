/*
 * =====================================================================================
 *
 *       Filename:  MinHeap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 02:57:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
namespace yevent
{
    typedef struct MinHeapEntry {
        int index;
        void *data;
    };
    class MinHeap
    {
    public:
        MinHeap(type): heapType_(type), pHeap_(NULL), size_(0), capacity_(0) 
        { }
        ~MinHeap(){ if (mht->p) free(pHeap_);}
        inline void Init();
        MinHeapEntry *top() { return size_?pHeap_[0]:0;}
        inline MinHeapEntry *pop();
        unsigned size() { return size_;}
        inline int push(MinHeapEntry *entry);
    private:
        inline int shiftUp(unsigned holeIndex);
        inline int shiftDown(unsigned holeIndex);
        inline int resize(unsigned n);
        MinHeapType *heapType_;
        MinHeapEntry **pHeap_;
        unsigned size_;
        unsigned capacity_;
    };

    inline void MinHeap::Init()
    {
        pHeap_ = malloc(MIN_HEAP_INITIAL * sizeof(MinHeapEntry));
        if (pHeap_ == NULL)
            return NULL;
        capacity_ = MIN_HEAP_INITIAL;
    }


}
