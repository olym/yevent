/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2012 03:24:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minheap.h"

int compare(void *privdata, const char *val1, const void *val2)
{
    return strcmp(val1, val2) > 0;
}
int main(int argc, const char *argv[])
{

    int i;
    min_heap_entry *get;

    min_heap_type *type = malloc(sizeof(min_heap_type));
    if (type == NULL)
        return -1;
    type->compare = compare;
    min_heap_t *mht = min_heap_create(type, NULL);
    min_heap_entry entry1;
    min_heap_entry entry2;
    min_heap_entry entry3;
    min_heap_entry entry4;

    entry1.data = strdup("723");
    min_heap_push(mht, &entry1);


    entry2.data = strdup("456");
    min_heap_push(mht, &entry2);


    entry3.data = strdup("45");
    min_heap_push(mht, &entry3);


    entry4.data = strdup("56");
    min_heap_push(mht, &entry4);

    for (i = 0; i < 4; i++) {
        get = min_heap_pop(mht);
        printf("data:%s\n", (char *)get->data);
    }
    //min_heap_entry *get = min_heap_pop(mht);
    //printf("data:%s\n", (char *)get->data);
    //get = min_heap_pop(mht);
    //printf("data:%s\n", (char *)get->data);
    //get = min_heap_pop(mht);
    //printf("data:%s\n", (char *)get->data);
    //get = min_heap_pop(mht);
    //printf("data:%s\n", (char *)get->data);
    return 0;
}
