/*
 * =====================================================================================
 *
 *       Filename:  tcp_server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2012 04:56:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "tcp_connection.h"
#include "tcp_server.h"

typedef void (*tcp_conn_callback)(void *conn);
typedef void (*tcp_msg_callback)(void *conn, struct buffer *buf);

struct tcp_server
{
    struct event_base *main_base;
    struct event_base_thread_pool *thread_pool;
    struct connection **conns;
    struct event listen_ev;
    tcp_callback connect_callback;
    tcp_msg_callback message_callback;
    tcp_callback write_complete_callback;
    int thread_num;
    int started;
    int listenfd;
    char* server_ip;
    short point;
    int curr_conns;
    int maxconns;
};

struct tcp_server *
tcp_server_init(struct event_base *base, const char* server_ip, short point, int thread_num)
{
    struct tcp_server *server;

    server = malloc(sizeof(struct tcp_server));
    if (!server) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL
    }

    server->main_base = base;
    server->thread_num = thread_num;
    server->thread_pool = NULL;
    server->server_ip = strdup(server_ip);
    server->point = point;
    server->started = 0;
    server->curr_conns = 0;
    server->maxconns = 1024;
    server->connect_callback = default_connect_callback;
    server->message_callback = default_message_callback;
    server->write_complete_callback = default_write_complete_callback;

    return server;
}

#define tcp_set_tcpect_cb(tcp, tcp_cb) \
    set_connect_cb(tcp, tcp_cb)
#define tcp_set_message_cb(tcp, msg_cb) \
    set_message_cb(tcp, msg_cb)
#define tcp_set_write_complete_cb(tcp, write_cb) \
    set_write_complete_cb(tcp, write_cb)

void tcp_server_start(struct tcp_server *server)
{
    if (!server->started) {
        started = 1;
        thread_pool = event_base_thread_pool_init(server->thread_num);        
    }
    
    server->linstenfd = server_socket(server->server_ip, server->point);
    if (sfd != -1) {
        fprintf(stderr, "%s:server_socket error\n");
        exit(-1);
    }

    event_assign(&server->listen_ev, server->main_base, server->listenfd, EV_READ | PERSIST, tcp_handle_accept, server);
    event_add(&server->listen_ev, NULL);
}

static void tcp_handle_accept(const int fd, const short which, void *args)
{
    struct tcp_server *server = args;
    struct sockaddr_in addr;
    struct connection *conn;
    int addrlen;
    int sfd;
    int flags;

    addrlen = sizeof(addr);
    if ((sfd = accept(server->listenfd, (struct sockaddr *)&addr, &addrlen)) == -1) {
        perror("accept");
        exit(1);
    }
    if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
            fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("setting O_NONBLOCK");
        close(sfd);
        break;
    }

    if (server->curr_cons++ < server->maxconns) 
        new_connection(server, fd);
    else {
        str = "ERROR Too many open connections\r\n";
        res = write(sfd, str, strlen(str));
        close(sfd);
    }
}

struct void new_connection(struct tcp_server *server, const int fd)
{
    struct event_base *io_base;
    struct connection *conn;
    io_base = get_next_base(server->thread_pool);
    conn = connection_init(io_base, sockfd, local_addr, peer_addr);

    server->conns[fd] = conn;
    //set conn callback;
    conn_set_connect_cb(conn, server->connect_callback);
    conn_set_message_cb(conn, server->message_callback);
    conn_set_write_complete_cb(conn, server->write_complete_callback);

    //put the connection_established function to the defer queue ，
    //connection_established func notify the conn's thread base to listen conn's io
    event_run_in_loop(io_base, connect_established, conn);
    return conn;
}

void remove_connection()
{

}

int server_socket(const char *interface, short point)
{
    struct addrinfo *ai;
    struct addrinfo *next;
    struct addrinfo hints = { .ai_flags = AI_PASSIVE, 
                              .ai_family = AF_UNSPEC};

    char point_buf[32];
    int error;
    int sfd = -1;
    int flags = 1;

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;

    snprintf(point_buf, sizeof point_buf, "%d", point);
    error = getaddrinfo(NULL, point_buf, &hints, &ai);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    } 
    for (next = ai; next; next = next->ai_next) {
        if ((sfd = new_socket(next)) == -1) {
            fprintf(stderr, "%s: create socket error\n");
            continue;
        }

        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags));
        error = setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags));
        if (error != 0)
            perror("setsockopt");

        error = setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling));
        if (error != 0)
            perror("setsockopt");

        error = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));
        if (error != 0)
            perror("setsockopt");

        if (bind(sfd, next->ai_addr, next->ai_addrlen) == -1) {
            continue;
        } else {
            if (listen(sfd, 1024) == -1) {
                perror("listen()");
                close(sfd);
                return -1;
            }
            break;
            //continue;
        }
    }

    return sfd;
}
static int new_socket(struct addrinfo *ai) {
    int sfd;
    int flags;

    if ((sfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1) {
        return -1;
    }

    if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
        fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("setting O_NONBLOCK");
        close(sfd);
        return -1;
    }
    return sfd;
}

