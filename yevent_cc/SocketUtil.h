/*
 * =====================================================================================
 *
 *       Filename:  SocketUtil.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月14日 14时51分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __SOCKET_UTIL_H
#define __SOCKET_UTIL_H

#include "InetSockAddr.h"

namespace yevent {
namespace socket {


int accept(int listenfd, InetSockAddr *peerAddr);
int bind(int listenfd, int port);
int listen(int listenfd);
int netNonBlock(int fd);
int netTcpNoDelay(int fd);
int createNoblockingSocket();
int close(int fd);
}
}
#endif
