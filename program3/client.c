/*************************************************************************
	> File Name: client.c
	> Author: ZHJ
	> Remarks: 
	> Created Time: Sun 14 Nov 2021 12:27:44 PM CST
 ************************************************************************/
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<unistd.h>

#include<stdio.h>  
#include<string.h>  
#include<stdlib.h>  
  
#include<event.h>  
#include<event2/listener.h>

int tcp_connect_server(const char* server_ip, int port);  
  
void cmd_msg_cb(int fd, short events, void* arg);  
void server_msg_cb(struct bufferevent* bev, void* arg);  
void event_cb(struct bufferevent *bev, short event, void *arg);  
  
int main()  
{  
    struct event_base *base = event_base_new();  
  
    struct bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    //监听终端输入事件  
    struct event* ev_cmd = event_new(base, STDIN_FILENO,  
                                     EV_READ | EV_PERSIST,  
                                     cmd_msg_cb, (void*)bev);  
  
    event_add(ev_cmd, NULL);  
  
    struct sockaddr_in server_addr;  
    memset(&server_addr, 0, sizeof(server_addr) );   
    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = 8888;  
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr,  
                               sizeof(server_addr));  
  
    bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);  
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  
   
    event_base_dispatch(base);  
  
    printf("finished \n");  
    return 0;  
}    

void cmd_msg_cb(int fd, short events, void* arg)  
{  
    char msg[1024];  
  
    int ret = read(fd, msg, sizeof(msg));  
    if( ret < 0 )  
    {  
        perror("read fail ");  
        exit(1);  
    }  
  
    struct bufferevent* bev = (struct bufferevent*)arg;  
  
    //把终端的消息发送给服务器端  
    bufferevent_write(bev, msg, ret);  
}  

void server_msg_cb(struct bufferevent* bev, void* arg)  
{  
    char msg[1024];  
  
    size_t len = bufferevent_read(bev, msg, sizeof(msg));  
    msg[len] = '\0';  
  
    printf("recv %s from server\n", msg);  
}  

void event_cb(struct bufferevent *bev, short event, void *arg)  
{  
  
    if (event & BEV_EVENT_EOF)  
        printf("connection closed\n");  
    else if (event & BEV_EVENT_ERROR)  
        printf("some other error\n");  
    else if( event & BEV_EVENT_CONNECTED)  
    {  
        printf("the client has connected to server\n");  
        return ;  
    }  
  
    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);  
  
    struct event *ev = (struct event*)arg;  
    event_free(ev);  
}

