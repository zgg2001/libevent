/*************************************************************************
	> File Name: main.c
	> Author: ZHJ
	> Remarks: 
	> Created Time: Fri 12 Nov 2021 06:49:50 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<signal.h>
#include<event.h>

int signal_count = 0;

void signal_handler(evutil_socket_t fd, short events, void *arg)
{
    struct event *ev = (struct event *)arg;
    printf("收到信号: %d\n", fd);
    signal_count++;
    if(signal_count >= 2)
    {
        event_del(ev);
    }
}

int main()
{
    //集合
    struct event_base *base = event_base_new();

    //创建事件
    struct event ev;

    //绑定
    event_assign(&ev, base, SIGINT, EV_SIGNAL | EV_PERSIST, signal_handler, &ev);

    //事件添加至集合
    event_add(&ev, NULL);

    //监听集合
    event_base_dispatch(base);

    //free
    event_base_free(base);
    return 0;
}

