/*
 * =====================================================================================
 *
 *       Filename:  MinHeap_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2012 12:36:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include "../MinHeap.h"

using namespace yevent;

int compare(const void *val1, const void *val2)
{
    return strcmp((const char*)val1, (const char*)val2) > 0;
}

int main(int argc, char *argv[])
{
    MinHeap minHeap(compare, NULL);
    minHeap.init();

    MinHeapEntry entry1;
    MinHeapEntry entry2;
    MinHeapEntry entry3;
    MinHeapEntry entry4;

    entry1.data = strdup("123");
    minHeap.push(&entry1);
    
    entry2.data = strdup("43");
    minHeap.push(&entry2);

    entry3.data = strdup("323");
    minHeap.push(&entry3);

    entry4.data = strdup("6");
    minHeap.push(&entry4);

    for(int i = 0; i < 4; i++) {
        MinHeapEntry *get = minHeap.pop();
        printf("data = %s\n", static_cast<char *>(get->data));
        printf("size = %d\n", minHeap.size());
        printf("capacity = %d\n", minHeap.capacity());
    }

    return 0;
}
