/*************************************************************************
	> File Name: write.c
	> Author: ZHJ
	> Remarks: 
	> Created Time: Thu 11 Nov 2021 05:28:02 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
    /*//创建有名管道(即文件)
    int ret = mkfifo("fifo.tmp", 00400 | 00200);
    if(ret == -1)
    {
        perror("mkfifo");
        exit(1);
    }*/

    int fd = open("fifo.tmp", O_WRONLY);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    char buf[32] = {0};
    while(1)
    {
        scanf("%s", buf);
        int ret = write(fd, buf, strlen(buf));
        if(ret == -1)
        {
            perror("write");
            exit(1);
        }

        if(!strcmp(buf, "bye"))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
    }

    return 0;
}
