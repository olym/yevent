/*
 * =====================================================================================
 *
 *       Filename:  minheap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/2012 11:19:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include "minheap.h"

min_heap_t * min_heap_create(min_heap_type *type, void *privDataPtr)
{
    min_heap_t *mht;

    if ((mht = malloc(sizeof(min_heap_t))) == NULL)
        return NULL;
    
    mht->p = malloc(MIN_HEAP_INITIAL * sizeof(min_heap_entry *));
    if (mht->p == NULL)
        return NULL;
    mht->type = type;
    mht->privdata = privDataPtr;
    mht->n = 0;
    mht->a = MIN_HEAP_INITIAL;
    return mht;
}
inline void min_heap_free(min_heap_t *mht)
{
    if (mht->p)
        free(mht->p);
}
inline min_heap_entry* min_heap_top(min_heap_t *mht)
{
    return mht->n ? mht->p[0] : 0;
}
inline unsigned min_heap_size(min_heap_t *mht)
{
    return mht->n;
}
min_heap_entry *min_heap_pop(min_heap_t *mht)
{
    if (mht->n) {
        struct min_heap_entry *entry = mht->p[0];
        mht->p[0] = mht->p[mht->n-1];
        mht->n--;
        min_heap_shift_down(mht, 0);
        entry->index = -1;
        return entry;
    }
    return NULL;
}
int min_heap_push(min_heap_t *mht, min_heap_entry *entry)
{
    if (min_heap_resize(mht, mht->n+1) == -1)
        return -1;
    mht->p[mht->n] = entry;
    entry->index = min_heap_shift_up(mht, mht->n++);
    //printf("%s:a = %d\n", __func__, entry->index);
    return 0;
}

int min_heap_shift_up(min_heap_t *mht, unsigned hole_index)
{
    if (hole_index == 0)
        return 0;
    unsigned parent_index = heap_parent(hole_index),
             min_index = hole_index;
    min_heap_entry *tmp;

    if (parent_index >= 0 && min_heap_elem_greater(mht, mht->p[parent_index], mht->p[min_index])) {
        tmp = mht->p[min_index];
        mht->p[min_index] = mht->p[parent_index];
        mht->p[parent_index] = tmp;

        return min_heap_shift_up(mht, parent_index);
    }
    return hole_index;
}
int min_heap_shift_down(min_heap_t *mht, unsigned hole_index)
{
    unsigned min_index = hole_index,
             left_index = heap_left(hole_index), 
             right_index = heap_right(hole_index);
    unsigned heap_size = mht->n;
    min_heap_entry *tmp;

    if (left_index < heap_size && min_heap_elem_greater(mht, mht->p[min_index], mht->p[left_index]))
        min_index = left_index;
    if (right_index < heap_size && min_heap_elem_greater(mht, mht->p[min_index], mht->p[right_index]))
        min_index = right_index;
    if (min_index != hole_index) {
        tmp = mht->p[min_index];
        mht->p[min_index] = mht->p[hole_index];
        mht->p[hole_index] = tmp;

        return min_heap_shift_down(mht, min_index);
    }
    return hole_index;

}

int min_heap_resize(min_heap_t *mht, unsigned n)
{
    if (mht->a < n) {
        unsigned a = mht->a ? mht->a * 2 : 8;
        min_heap_entry **p;
        if (a < n)
            a = n;
        if (!(p = (min_heap_entry **)realloc(mht->p, a)))
            return -1;
        mht->p = p;
        mht->a = a;
    }

    return 0;
}
