/*************************************************************************
	> File Name: server.c
	> Author: ZHJ
	> Remarks: 
	> Created Time: Sat 13 Nov 2021 04:47:47 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
//memset
#include<string.h>
//libevent
#include<event.h>
#include<event2/listener.h>//evconnlistener_new_bind宏定义
//socket
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//bufferevent对象回调函数 读事件
void read_cb(struct bufferevent* bev, void* ctx)
{
    //int fd = *(int*)ctx;
    char buf[128] = {0};
    size_t ret = bufferevent_read(bev, buf, sizeof(buf));
    if(ret < 0)
    {
        printf("bufferevent_read() error\n");
    }
    else
    {
        printf("read msg: %s", buf);
    }
}

//bufferevent对象回调函数 异常事件
void event_cb(struct bufferevent* bev, short what, void* ctx)
{
    if(what & BEV_EVENT_EOF)
    {
        printf("客户端下线\n");
        bufferevent_free(bev);
    }
    else
    {
        printf("未知错误\n");
    }
}

// evconnlistener_new_bind的回调函数 
void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg)
{
    printf("接收到新的socket连接, socket fd = %d\n", fd);
    struct event_base* base = arg;
    //对已存在的socket创建bufferevent对象
    //参数: 事件集合(主函数中), fd(此连接的socket fd), 可选参数(释放bufferevent对象就关闭连接)
    struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(NULL == bev)
    {
        printf("bufferevent_socket_new() error\n");
        exit(1);
    }
    
    //为bufferevent对象设置回调函数
    //参数: bufferevent对象, 读事件cb, 写事件cb, 异常事件cb, 传参
    //注意: 此处整体思路类似于select的选择
    bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL/*&fd*/);

    //使能bufferevent对象
    //参数: bufferevent对象, 读操作 | 写操作
    bufferevent_enable(bev, EV_READ);
}

int main()
{
    //新建事件集合
    struct event_base* base = event_base_new();
    if(NULL == base)
    {
        printf("event_base_new() error\n");
        exit(1);
    }

    //创建socket addr结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = 8888;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    //创建监听对象，监听TCP连接
    //参数: 监听集合, 回调函数, cb传参, 可选参数(释放监听对象关闭socket | 端口重复使用), 
    //      监听队列, addr结构体, addr结构体大小
    //注意：此处的回调函数为建立连接后调用，用来实现通知等操作，此时fd已添加进base中.
    struct evconnlistener* listener = 
    evconnlistener_new_bind(base, listener_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 
                            10, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(NULL == listener)
    {
        printf("evconnlistener_new_bind() error\n");
        exit(1);
    }

    //监听集合中的事件
    event_base_dispatch(base);

    //释放
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}
