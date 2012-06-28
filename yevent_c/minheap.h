/*
 * =====================================================================================
 *
 *       Filename:  minheap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2012 02:17:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _MINHEAP_H

typedef struct min_heap_entry {
    int index;
    void *data;
} min_heap_entry;

typedef struct min_heap_type
{
    int (*compare)(void *privdata, const void *val1, const void *val2);
    void (*val_destructor)(void *privdata, void *obj);
} min_heap_type;
typedef struct min_heap
{
    min_heap_type *type;
    void *privdata;
    min_heap_entry **p;
    unsigned n, a;
} min_heap_t;
#define MIN_HEAP_INITIAL 8

#define heap_left(i) \
    (2*(i)+1)
#define heap_right(i) \
    (2*(i)+2)
#define heap_parent(i) \
    (((i)-1)/2)

#define min_heap_elem_greater(d, entry1, entry2) \
    (((d)->type->compare) ? \
        (d)->type->compare((d)->privdata, entry1->data, entry2->data) : \
        (entry1->data) > (entry2->data))
min_heap_t * min_heap_create(min_heap_type *type, void *privDataPtr);
inline void min_heap_free(min_heap_t *mht);
inline min_heap_entry* min_heap_top(min_heap_t *mht);
inline unsigned min_heap_size(min_heap_t *mht);
min_heap_entry *min_heap_pop(min_heap_t *mht);
int min_heap_push(min_heap_t *mht, min_heap_entry *entry);
int min_heap_shift_up(min_heap_t *mht, unsigned hole_index);
int min_heap_shift_down(min_heap_t *mht, unsigned hole_index);
int min_heap_resize(min_heap_t *mht, unsigned n);

#endif /* _MINHEAP_H */
