/*
 * =====================================================================================
 *
 *       Filename:  epoll_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/2012 11:21:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <pthread.h>
#define MAX_EVENTS 10

void *epoll_thread(void *args)
{

    sigset_t mask;
    int sfd;
    int epollfd;
    ssize_t s;
    struct epoll_event ev, events[MAX_EVENTS];
    //struct signalfd_siginfo fdsi;

    //sigemptyset(&mask);
    //sigaddset(&mask, SIGINT);
    //sigaddset(&mask, SIGQUIT);

    //if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    //    perror("signalfd");
    //    exit(0);
    //}

    //sfd = signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
    //if (sfd == -1)
    //    exit(0);
    
    sfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(0);
    } 
    ev.data.fd = sfd;
    ev.events = EPOLLIN | EPOLLET;
    printf("thread epoll_ctl\n");
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
        perror("epoll_ctl");
        exit(0);
    }
    
    int nfds;
    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(0);
        }

        printf("thread get nfds = %d\n", nfds);

        int n;
        long int buf;
        for (n = 0; n < nfds; n++) {
            if (events[n].data.fd == sfd) {
                int s = read(sfd, &buf, sizeof(struct signalfd_siginfo));
                printf("thread read buf = %ld\n", buf);
                //if (s != sizeof(struct signalfd_siginfo))
                //    perror("read");
                //if (fdsi.ssi_signo == SIGINT)
                //    printf("Got SIGINT\n");
                //else if (fdsi.ssi_signo == SIGQUIT) {
                //    printf("got SIGQUIT\n");
                //    exit(0);
                //} else {
                //    printf("read unexpected signal\n");
                //}
            }
        }
    }
    return 0;
}
int main(int argc, const char *argv[])
{
    sigset_t mask;
    int sfd;
    int epollfd;
    ssize_t s;
    struct epoll_event ev, events[MAX_EVENTS];
    pthread_t tid;
    //struct signalfd_siginfo fdsi;

    //sigemptyset(&mask);
    //sigaddset(&mask, SIGINT);
    //sigaddset(&mask, SIGQUIT);

    //if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    //    perror("signalfd");
    //    exit(0);
    //}

    //sfd = signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
    sfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (sfd == -1)
        exit(0);
    epollfd = epoll_create(10);
    if (epollfd == -1) {
        perror("epoll_create");
        exit(0);
    } 
    ev.data.fd = sfd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &ev) == -1) {
        perror("epoll_ctl");
        exit(0);
    }
    pthread_create(&tid, NULL, epoll_thread, NULL); 
    int nfds;
    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(0);
        }

        printf("get nfds = %d\n", nfds);

        int n;
        long int buf;
        for (n = 0; n < nfds; n++) {
            if (events[n].data.fd == sfd) {
                int s = read(sfd, &buf, sizeof(struct signalfd_siginfo));
                printf("read buf = %ld\n", buf);
                //if (s != sizeof(struct signalfd_siginfo))
                //    perror("read");
                //if (fdsi.ssi_signo == SIGINT)
                //    printf("Got SIGINT\n");
                //else if (fdsi.ssi_signo == SIGQUIT) {
                //    printf("got SIGQUIT\n");
                //    exit(0);
                //} else {
                //    printf("read unexpected signal\n");
                //}
            }
        }
    }
    return 0;
}
