/*
 * =====================================================================================
 *
 *       Filename:  Thread_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/25/2012 12:06:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "../Thread.h"

using namespace yevent;

void *print(void *args)
{
    while (1){
        printf("pthread = %d\n", (int)args);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    Thread thread(print, (void *)123, "thread");
    thread.start();
    sleep(20);
    return 0;
}
