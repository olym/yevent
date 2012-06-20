/*
 * =====================================================================================
 *
 *       Filename:  SocketUtil.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月14日 11时37分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "SocketUtil.h"

using namespace yevent;

namespace yevent{
namespace socket
{

int accept(int listenfd, InetSockAddr *peerAddr)
{
    int fd;
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
    while(1) {
        fd = accept(listenfd,(struct sockaddr *)&sa,&salen);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                perror("accept error\n");
                return -1;
            }
        }
        break;
    }
    
    peerAddr->setSockAddr(sa);
    return fd;
}

int bind(int s, int port)
{
    struct sockaddr_in sa;

    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (::bind(s,(struct sockaddr *)&sa,sizeof(sa)) == -1) {
        perror("bind error\n");
        close(s);
        return -1;
    }
    return 0;
}

int listen(int s)
{
    if (::listen(s, 1024) == -1) { /* the magic 511 constant is from nginx */
        perror("listen error");
        close(s);
        return -1;
    }

    return 0;
}

int netNonBlock(int fd)
{
    int flags;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        fprintf(stderr, "fcntl(F_GETFL): %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(stderr, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return -1;
    }
    return 0;
}

int netTcpNoDelay(int fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1)
    {
        fprintf(stderr, "setsockopt TCP_NODELAY: %s", strerror(errno));
        return -1;
    }
    return 0;
}
int createNoblockingSocket()
{
    int s, on = 1;
    if ((s = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("create socket error");
        return -1;
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        fprintf(stderr, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return -1;
    }
    
    netNonBlock(s);
    return s;
}
int close(int fd)
{
    if (::close(fd) == -1) {
        fprintf(stderr, "close: %s", strerror(errno));
    }
}
}
}
