/*
 * =====================================================================================
 *
 *       Filename:  test_thread_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2012 03:11:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/types.h>
#include "thread_pool.h"
void *print()
{
    printf("current tid = %d\n", tid());
    return NULL;
}
void *print_pool(void *args)
{
    printf("%s: tid = %d\n", (char*)args, tid());
    sleep(1);
}
int main(int argc, const char *argv[])
{
    struct thread_pool *pool;
    int i;
    pool = thread_pool_init("test", 2);
    if (pool == NULL)
        return -1;
    thread_pool_run(pool, print, NULL);
    thread_pool_start(pool);
    for (i = 0; i < 100; i++) {
        thread_pool_run(pool, print_pool, pool->name);
    }

    sleep(10);
    thread_pool_stop(pool);
    printf("thread pool stop\n");
    return 0;
}
