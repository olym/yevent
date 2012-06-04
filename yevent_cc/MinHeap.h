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
#ifndef __MINHEAP_H
#define __MINHEAP_H

#include <stdlib.h>

namespace yevent
{

#define heap_left(i) \
    (2*(i)+1)
#define heap_right(i) \
    (2*(i)+2)
#define heap_parent(i) \
    (((i)-1)/2)
#define MIN_HEAP_INITIAL 8

typedef int (*CompareFunc)(void *val1, void *val2);
typedef void (*ElementDestructor)(void *obj);

template<class T>
    class MinHeap
    {
    public:
        MinHeap(CompareFunc compareFunc, ElementDestructor elemDestructor): 
            compareFunc_(compareFunc),
            elemDestructor_(elemDestructor),
            pHeap_(NULL), size_(0), capacity_(0) 
        { }
        ~MinHeap()
        { 
            if (pHeap_) {
                if (elemDestructor_) {
                    for (int i = 0; i < size(); i++) {
                        elemDestructor_(pHeap_[i]);
                    }
                }
                free((void*)pHeap_);
            }
        }
        inline void init();
        inline T pop();
        inline int push(T entry);
        //T *operator[](const int index) { return pHeap_[index];}
        T getEntry(int i) { return pHeap_[i];}
        unsigned size() { return size_;}
        unsigned capacity() { return capacity_;}
        bool empty() {return size_ == 0;}
        T top() { return size_?pHeap_[0]:0;}

    private:
        inline int shiftUp(unsigned holeIndex);
        inline int shiftDown(unsigned holeIndex);
        inline int resize(unsigned n);
        bool minHeapElemGreater(T entry1, T entry2)
        {
            return compareFunc_ ? compareFunc_(entry1, entry2) : (entry1 > entry2);
        }
        ElementDestructor elemDestructor_;
        CompareFunc compareFunc_;
        T *pHeap_;
        unsigned size_;
        unsigned capacity_;
    };

template<class T>
    inline void MinHeap<T>::init()
    {
        pHeap_ = (T *)malloc(MIN_HEAP_INITIAL * sizeof(T));
        if (pHeap_ == NULL) {
            return;
        }
        capacity_ = MIN_HEAP_INITIAL;
    }

template<class T>
    inline T MinHeap<T>::pop()
    {
        if (!empty()) {
            T entry = pHeap_[0];
            pHeap_[0] = pHeap_[size_-1];
            size_--;
            shiftDown(0);
            return entry;
        }
        return NULL;
    }
    
template<class T>
    inline int MinHeap<T>::push(T entry)
    {
        if (resize(size_+1) == -1)
            return -1;
        pHeap_[size_] = entry;
        shiftUp(size_++);

        return 0;
    }

template<class T>
    inline int MinHeap<T>::shiftUp(unsigned holeIndex)
    {
        if (holeIndex == 0)
            return 0;
        unsigned parent_index = heap_parent(holeIndex),
                 min_index = holeIndex;
        T tmp;

        if (parent_index >= 0 && minHeapElemGreater(pHeap_[parent_index], pHeap_[min_index])) {
            tmp = pHeap_[min_index];
            pHeap_[min_index] = pHeap_[parent_index];
            pHeap_[parent_index] = tmp;

            return shiftUp(parent_index);
        }
        return holeIndex;
    }

template<class T>
    inline int MinHeap<T>::shiftDown(unsigned holeIndex)
    {
        unsigned min_index = holeIndex,
                 left_index = heap_left(holeIndex), 
                 right_index = heap_right(holeIndex);
        unsigned heap_size = size_;
        T tmp;

        if (left_index < heap_size && minHeapElemGreater(pHeap_[min_index], pHeap_[left_index]))
            min_index = left_index;
        if (right_index < heap_size && minHeapElemGreater(pHeap_[min_index], pHeap_[right_index]))
            min_index = right_index;
        if (min_index != holeIndex) {
            tmp = pHeap_[min_index];
            pHeap_[min_index] = pHeap_[holeIndex];
            pHeap_[holeIndex] = tmp;

            return shiftDown(min_index);
        }
        return holeIndex;

    }

template<class T>
    inline int MinHeap<T>::resize(unsigned n)
    {
        if (capacity_ < n) {
            unsigned a = capacity_ ? capacity_ * 2 : 8;
            T *p;
            if (a < n)
                a = n;
            if (!(p = (T *)realloc(pHeap_, a)))
                return -1;
            pHeap_ = p;
            capacity_ = a;
        }

        return 0;
    }
}
#endif
