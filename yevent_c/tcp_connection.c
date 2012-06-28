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

#define NoneEvent   0x00
#define ReadEvent   0x01
#define WriteEvent  0x02

struct connection
{
    struct event_base *base;
    int fd;
    struct event read_ev;
    struct event write_ev;
    struct buffer *input_buffer;
    struct buffer *output_buffer;
    tcp_callback connect_callback;
    tcp_msg_callback message_callback;
    tcp_callback write_complete_callback;
    States conn_state;
    int rw_state;
};

struct connection *
connection_init()
{
    conn->rw_state = NoneEvent;
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
    struct connection *conn = args;
    struct buffer *buff = conn->output_buffer;
    if (conn_is_writing(conn)) {
        ssize_t n = write(conn->fd, buffer_peek(buff),
                buffer_readable_bytes(buff));
        if (n > 0) {
            buffer_retrieve(buff, n);
            if (buffer_readable_bytes(buff) == 0) {
                disable_writing(conn);
                if (conn->write_complete_callback) {
                    conn->write_complete_callback(conn);
                }
                if (conn->conn_state == Disconnecting) {
                    shutdown(conn->fd, SHUT_WR);
                }
            }
            else {
                fprintf(stdout, "%s: I am going to write more data\n", __func__);
            }
        }
        else {
            fprintf(stderr, "TcpConnection::handleWrite\n");
        }
    } else {
        fprintf(stdout, "Connection is down, no more writing\n");
    }
}
void handle_close(void *args)
{
    struct connection *conn = args;

}
void connect_established(void *args)
{
    struct connection *conn = args;

    set_stats(Connected);

    conn->rw_state |= ReadEvent;
    event_assign(&conn->read_ev, conn->base, conn->fd, EV_READ | EV_PERSIST, handle_read, conn);
    event_add(&conn->read_ev, NULL);

    if (conn->connect_callback)
        conn->connect_callback(conn);
}

void enable_writing(struct connection *conn)
{
    conn->rw_state |= WriteEvent;

    event_assign(&conn->write_ev, conn->base, conn->fd, EV_WRITE | EV_PERSIST, handle_write, conn);
    event_add(&conn->write_ev, NULL);
}

void disable_writing(struct connection *conn)
{
    conn->rw_state &= ~WriteEvent;
    event_del(&conn->write_ev);
}

void connect_send(struct connecttion *conn, void *data, int len)
{
    if (conn->conn_state == Connected) {
        if (EVBASE_IS_IN_LOOP_THREAD(conn->base))
            connect_send_inloop(conn, data, len);
    }
}
#define conn_is_writing(conn) \
    ((conn)->rw_state & WriteEvent)

void connect_send_inloop(struct connection *conn, const void * data, size_t len)
{
    ssize_t nwrote = 0;
    
    // if no thing in output queue, try writing directly
    if(conn_is_writing(conn) && buffer_readable_bytes(conn->output_buffer) == 0) {
        nwrote = write(conn->fd, data, len);
        if (nwrote == len && conn->write_complete_callback) {
            conn->write_complete_callback(conn);    
        }
        if (nwrote < 0) {
            nwrote = 0;
            //错误不是非阻塞下的中断
            if (errno != EWOULDBLOCK) {
                fprintf(stderr, "%s:write error\n", __func__);
            }
        }
    }

    //为什么不继续调用write将剩余没写完的字节写完呢，而是放到output_buffer中
    //等待下次epoll调用handle_write去写完呢？
    //这样为了快速进入到epool_wait状态，不阻塞其他event
    if (nwrote < len) {
        buffer_append(conn->output_buffer, (const char *)(data)+nwrote, len - nwrote);
        if (!conn_is_writing(conn)) 
            enable_writing(conn);
    }
}

void connection_destroy()
{

}
