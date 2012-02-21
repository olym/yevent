/*
 * =====================================================================================
 *
 *       Filename:  tcp_connection.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2012 12:18:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

struct connection
{
    struct event_base *base;
    struct event *ev;
    int fd;
    struct buffer *input_buffer;
    struct buffer *output_buffer;
};

struct connection *
connection_init()
{

}

void handle_read(void *args)
{

}
void handle_write(void *args)
{

}

void connect_established()
{

}

void connect_send(struct connecttion *conn, void *data, int len)
{

}
