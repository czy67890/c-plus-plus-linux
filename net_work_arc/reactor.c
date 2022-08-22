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
#define MAX_EPOLL_EVENT 1024
#define POLL_SIZE 1024


typedef int NCALLBACK(int fd, int event, void *arg);
int ncreator_del(int fd, NCALLBACK cb, int event, void *arg);
//设置回调函数
// epoll池中管理的对象
typedef struct nitem
{
    int fd;
    //事件，通过int来表示
    int events;
    void *arg;
    //一个回调函数的情况,epoll处理时判断可读还是可写

    //两个回调函数，两者可以并存
    NCALLBACK *readcb;
    NCALLBACK *writecb;
    NCALLBACK *accpetcb;
    // ASCII大于0,只需要存储大于0的情况
    unsigned char sbuff[MAX_LEN];
    int slenght;
    unsigned char rbuff[MAX_LEN];
    int rlength;
} nitem;

typedef struct itemblock
{
    struct itemblock *next;
    nitem *items;
} itemblock;

//链表+数组方式来存储事件
struct reactor
{
    int epfd;
    itemblock *first; 
};


int nreactor_set_event(int fd, NCALLBACK cb, int event, void *arg);
int init_reactor(struct reactor *r)
{
    int epfd = epoll_create(1);
    // fd与item的关系为直接的数组下标
    r->first = (struct itemblock *)malloc(sizeof(itemblock));
    r->epfd = epfd;
    if (r->first == NULL)
    {
        close(epfd);
        return -2;
    }
    memset(r->first, 0, sizeof(itemblock));
    //分配事件
    r->first->items = (nitem *)malloc(MAX_EPOLL_EVENT * sizeof(nitem));
    if (r->first->items == NULL)
    {   
        close(epfd);
        return -2;
    }
    memset(r->first->items, 0,(MAX_EPOLL_EVENT * sizeof(nitem)));
    r->first->next = NULL;
    return 0;
}
//单例模式的全局reactor
struct reactor *g_reactor = NULL;
struct reactor *getinstance(void)
{
    if (g_reactor == NULL)
    {
        g_reactor = (struct reactor *)malloc(sizeof(struct reactor));
        if (NULL == g_reactor)
        {
            return NULL;
        }
        memset(g_reactor, 0, sizeof(struct reactor));
        if (init_reactor(g_reactor) < 0)
        {
           return NULL;
        }
    }
    return g_reactor;
}
#define NO_SET_CB 0
#define READ_CB 1
#define WRITE_CB 2
#define ACCEPT_CB 3

int read_callback(int fd,int event,void *arg);
int write_callback(int fd,int event,void *arg){
    struct reactor * r = getinstance();
    unsigned char *sbuff = r->first->items[fd].sbuff;
    int length = r->first->items[fd].slenght;
    int ret = send(fd,sbuff,length,0);
    if(ret < length){
        nreactor_set_event(fd,write_callback,WRITE_CB,NULL);
    }
    else{
        nreactor_set_event(fd,read_callback,READ_CB,NULL);
    }
    return 0;
}

int read_callback(int fd,int event,void *arg){
    struct reactor * r = getinstance();
    unsigned char *buffer = r->first->items[fd].rbuff;
#if 0
    while(index < MAX_LEN){
        ret = recv(fd,buffer+index,MAX_LEN - index,0);
        if(ret == -1){
            //break;
        }
        else if(ret > 0){
            index += ret;
            //加入到
            nreactor_set_event(fd,read_callback,READ_CB,NULL);
        }
        else{
           // break;
        }
    //}
#endif
    int ret = recv(fd,buffer,MAX_LEN,0);
    if(ret == 0){
        ncreator_del(fd,NULL,0,NULL);
        close(fd);
    }
    else if(ret > 0){
        unsigned char * sbuffer = r->first->items[fd].sbuff;
        memcpy(sbuffer,buffer,ret);
        r->first->items[fd].slenght = ret;
        printf("read cb : %s\n",sbuffer);
        nreactor_set_event(fd,write_callback,WRITE_CB,NULL);
    }
}

int accpet_callback(int fd, int event, void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int connfd;
    if ((connfd = accept(fd, (struct sockaddr *)&client, &len)) == -1)
    {
        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    printf("accept sucess\n");
    nreactor_set_event(connfd,read_callback,READ_CB,NULL);
}

int init_server(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock != -1);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    assert(sock >= 0);
    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(sock, 10);
    assert(ret != -1);
    return sock;
}

int nreactor_set_event(int fd, NCALLBACK cb, int event, void *arg)
{
    struct reactor *r = getinstance();
    struct epoll_event ev = {0};
    

    if (event == READ_CB)
    {
        r->first->items[fd].fd = fd;
        r->first->items[fd].readcb = cb;
        r->first->items[fd].arg = arg;
        ev.events = EPOLLIN;
    }
    else if (event == WRITE_CB)
    {
        r->first->items[fd].fd = fd;
        r->first->items[fd].writecb = cb;
        r->first->items[fd].arg = arg;
        ev.events = EPOLLOUT;
    }
    else if (event == ACCEPT_CB)
    {
        r->first->items[fd].fd = fd;
        r->first->items[fd].accpetcb = cb;
        r->first->items[fd].arg = arg;
        ev.events = EPOLLIN;
    }
    ev.data.ptr = &r->first->items[fd]; 
    if(r->first->items[fd].events == NO_SET_CB){
        if (epoll_ctl(r->epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
			printf("epoll_ctl EPOLL_CTL_ADD failed, %d\n", errno);
			return -1;
		}
		r->first->items[fd].events = event;
    }
    else if(r->first->items[fd].events != event){
        if (epoll_ctl(r->epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
			printf("epoll_ctl EPOLL_CTL_MOD failed\n");
			return -1;
		}
		r->first->items[fd].events = event;
    }
    return 0;
}

int reactor_loop(int listenfd){ 
    struct epoll_event events[MAX_EPOLL_EVENT] = {0};
    struct reactor* r = getinstance();
    while (1)
    {
        int nready = epoll_wait(r->epfd, events, POLL_SIZE, 5);
        if (nready == -1)
        {
            continue;
        }
        int i = 0;
        for (i = 0; i < nready; ++i)
        {   
            struct nitem *item = (struct nitem *)events[i].data.ptr;
            int connfd = item->fd;
            if (connfd == listenfd)
            {
                item->accpetcb(listenfd,0,NULL);
            }
            else{
                if(events[i].events & EPOLLIN){
                    item->readcb(connfd,0,NULL);
                }
                if(events[i].events &EPOLLOUT){
                    item->writecb(connfd,0,NULL);
                }
            }
        }
    }
    return 0;
}

int ncreator_del(int fd, NCALLBACK cb, int event, void *arg){
    struct reactor * r = getinstance();
    struct epoll_event ev = {0};
    ev.data.ptr = arg;
    epoll_ctl(r->epfd,EPOLL_CTL_DEL,fd,&ev);
    r->first->items[fd].events = 0;
    return 0;
}

int main(int argc, char *argv[])
{
    //两个fdset，一个注册给内核，一个用户使用
    char buff[MAX_LEN];
    //封装到init_server函数中
    int listenfd = init_server(8080);
    //将监听端口连接加入到epoll中
    nreactor_set_event(listenfd, accpet_callback, ACCEPT_CB, NULL);
    reactor_loop(listenfd);
    close(listenfd);
    return 0;
}