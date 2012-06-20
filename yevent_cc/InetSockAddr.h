/*
 * =====================================================================================
 *
 *       Filename:  InetSockAddr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月01日 10时37分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __INET_SOCKADDR_H
#define __INET_SOCKADDR_H

#include <stdio.h>
#include <netinet/in.h>
#include <strings.h>
#include <string>
#include <arpa/inet.h>
namespace yevent {
    class InetSockAddr {
        public:
            InetSockAddr(uint16_t port) 
            {
                bzero(&addr_, sizeof addr_);
                addr_.sin_family = AF_INET;
                addr_.sin_addr.s_addr = ::htonl(INADDR_ANY);
                addr_.sin_port = ::htons(port);
            }
            InetSockAddr(std::string ip, uint16_t port) 
            {
                bzero(&addr_, sizeof addr_);
                addr_.sin_family = AF_INET;
                addr_.sin_port = ::htons(port);
                if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0) {
                    printf("inet_pton error\n");
                }
            }
            InetSockAddr(const struct sockaddr_in& addr)
                : addr_(addr)
            { }

            int getPort() {
                return ntohs(addr_.sin_port);
            }
            std::string getHost() {
                std::string ip(inet_ntoa(addr_.sin_addr));
                return ip;
            }
            const struct sockaddr_in& getSockAddr() const { return addr_; }
            void setSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }
        private:
            struct sockaddr_in addr_;
    };
}
#endif
