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

struct tcp_server
{
    struct event_base *base;
    int thread_num;
    struct event_base_thread_pool *thread_pool;
    struct connection **conns;
    int started;
};

struct tcp_server *
tcp_server_init(int thread_num)
{
    server->thread_num = thread_num;
    server->thread_pool = NULL;
    int started = 0;
}

void tcp_server_start(struct tcp_server *server)
{
    if (!server->started) {
        started = 1;
        thread_pool = event_base_thread_pool_init(server->thread_num);        
    }

    //设置监听
}

void new_connection()
{
    struct event_base *io_base;
    struct connection *conn;
    io_base = get_next_base(server->thread_pool);
    conn = connection_init(io_base, sockfd, local_addr, peer_addr);
    //set conn callback;

    //put the connection_established function to the defer queue ，
    //connection_established func notify the conn's thread base to listen conn's io
}

void remove_connection()
{

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

