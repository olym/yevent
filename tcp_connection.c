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

enum States { Disconnected, Connecting, Connected };

#define conn_set_connect_cb(conn, conn_cb) \
    set_connect_cb(conn, conn_cb)
#define conn_set_message_cb(conn, msg_cb) \
    set_message_cb(conn, msg_cb)
#define conn_set_write_complete_cb(conn, write_cb) \
    set_write_complete_cb(conn, write_cb)

struct connection
{
    struct event_base *base;
    struct event *ev;
    int fd;
    struct buffer *input_buffer;
    struct buffer *output_buffer;
    tcp_callback connect_callback;
    tcp_msg_callback message_callback;
    tcp_callback write_complete_callback;
    States conn_state;
};

struct connection *
connection_init()
{

}

void
set_stats(struct connection *conn, States s)
{
    conn->conn_state = s;
}

void handle_read(void *args)
{
    struct connection *conn = args;
    ssize_t n;

    n = buffer_read(conn->fd, conn->input_buffer);
    if (n > 0)
        conn->message_callback(conn, conn->input_buffer);
    else (n == 0) {
        handle_close(conn);
    else
        handle_error(conn);

}
void handle_write(void *args)
{

}

void connect_established(void *args)
{
    struct connection *conn = args;
    struct event ev;

    set_stats(Connected);

    event_assign(&ev, base, conn->fd, EV_READ | EV_PERSIST, handle_read, conn);
    event_add(&ev, NULL);

    if (conn->connect_callback)
        conn->connect_callback(conn);
}

void connect_send(struct connecttion *conn, void *data, int len)
{

}
