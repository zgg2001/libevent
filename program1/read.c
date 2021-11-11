/*************************************************************************
	> File Name: read.c
	> Author: ZHJ
	> Remarks: 
	> Created Time: Thu 11 Nov 2021 04:57:07 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>   //open
#include<unistd.h>  //read
#include<stdlib.h>
#include<event.h>

//回调函数
void fifo_read(evutil_socket_t fd, short events, void *arg)
{
    char buf[32] = {0};
    int ret = read(fd, buf, sizeof(buf));
    if(ret == -1)
    {
        perror("read");
        exit(1);
    }
    printf("%s\n", buf);
}

int main()
{
    int ret = mkfifo("fifo.tmp", 00700);
    if(ret == -1)
    {
        perror("mkfifo");
        exit(1);
    }

    int fd = open("fifo.tmp", O_RDONLY);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    //初始化事件
    struct event ev;

    //初始化事件集合
    event_init();

    //初始化事件
    event_set(&ev, fd, EV_READ | EV_PERSIST, fifo_read, NULL);

    //添加事件
    event_add(&ev, NULL);

    //开始监听
    event_dispatch(); 

    return 0;
}
