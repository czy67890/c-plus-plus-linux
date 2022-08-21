#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>
#define MAX_LEN 1024
#define POLL_SIZE 1024

void *client_routine(void *arg)
{
    int connfd = *(int *)arg;
    char buff[MAX_LEN];
    while (1)
    {
        int n = recv(connfd, buff, n, 0);
        if (n > 0)
        {
            buff[n] = '\0';
            printf("recv msg from client :%s", buff);
            send(connfd, buff, n, 0);
        }
        else if (n == 0)
        {
            //关闭相应的资源
            close(connfd);
            break;
        }
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage:  ip_address port_number\n");
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(sock, 5);
    assert(ret != -1);
    //两个fdset，一个注册给内核，一个用户使用
    char buff[MAX_LEN];
    //循环侦听
    int connfd;
    int max_fd = sock;
    int n;
    /*while (1) {

        rset = rfds;
        wset = wfds;

        int nready = select(max_fd+1, &rset, &wset, NULL, NULL);


        if (FD_ISSET(sock, &rset)) { //

            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            if ((connfd = accept(sock, (struct sockaddr *)&client, &len)) == -1) {
                printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
                return 0;
            }

            FD_SET(connfd, &rfds);

            if (connfd > max_fd) max_fd = connfd;

            if (--nready == 0) continue;

        }

        int i = 0;
        for (i = sock+1;i <= max_fd;i ++) {

            if (FD_ISSET(i, &rset)) { //
                n = recv(i, buff, MAX_LEN, 0);
                if (n > 0) {
                    buff[n] = '\0';
                    printf("recv msg from client: %s\n", buff);
                    FD_SET(i, &wfds);

                    //reactor
                    //send(i, buff, n, 0);
                } else if (n == 0) { //

                    FD_CLR(i, &rfds);
                    //printf("disconnect\n");
                    close(i);

                }
                if (--nready == 0) break;
            }
            else if (FD_ISSET(i, &wset)) {
                printf("inter the send %d \n",i);
                send(i, buff, n, 0);
                FD_SET(i, &rfds);

                //这里要将写的fd清除出去，否则服务器会一直发
                FD_CLR(i,&wfds);
                if (--nready == 0) break;
            }
        }
    }*/

    /*struct pollfd fds[POLL_SIZE] = {0};
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    while (1)
    {
        int nready = poll(fds, max_fd, -1);
        if (fds[0].revents & POLLIN)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            if ((connfd = accept(sock, (struct sockaddr *)&client, &len)) == -1)
            {
                printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
                return 0;
            }
            fds[connfd].fd = connfd;
            fds[connfd].events = POLLIN;
            if (connfd > max_fd)
            {
                max_fd = connfd;
            }
            if (--nready == 0)
            {
                continue;
            }
        }

        int i = 0;
        for (int i = sock + 1; i <= max_fd; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                n = recv(i, buff, MAX_LEN, 0);
                if (n > 0)
                    buff[n] = '\0';
                fds[i].events |= POLLOUT;
            }
            else if (fds[i].revents & POLLOUT)
            {
                send(i, buff, n, 0);
            }
        }
    }*/

    //传一个大于0的参数即可
    //为了和早期的epoll_create兼容（采用数组，新版采用链表），所以还有size这个形参
    //创建epoll
    int epfd = epoll_create(1);
    //存储活跃的event
    struct epoll_event events[POLL_SIZE] = {0};

    //处理连接的事件
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    //将处理连接的端口加入进去
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
    while (1)
    {
        int nready = epoll_wait(epfd, events, POLL_SIZE, 5);
        if (nready == -1)
        {
            continue;
        }
        int i = 0;
        for (i = 0; i < nready; ++i)
        {   
            if (events[i].data.fd == sock)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                if ((connfd = accept(sock, (struct sockaddr *)&client, &len)) == -1)
                {
                    printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
                    return 0;
                }
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
            }
            else if(events[i].events & EPOLLIN)
            {   
                int n =  recv(events[i].data.fd,buff,MAX_LEN,0);
                //断开连接的情况
                if(n == 0){
                    epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,&events[i]);
                    continue;
                }
                buff[n] = '\0';
                ev.events = EPOLLOUT;
                ev.data.fd = events[i].data.fd;
                epoll_ctl(epfd,EPOLL_CTL_MOD,ev.data.fd,&ev);
            }
            else if(events[i].events & EPOLLOUT){
                send(events[i].data.fd,buff,n,0);
                events[i].events = EPOLLIN;
                epoll_ctl(epfd,EPOLL_CTL_MOD,events[i].data.fd,&events[i]);
            }
        }
    }
    close(sock);
    return 0;
}