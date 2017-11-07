//
//  main.c
//  TCP 回射服务器程序(并发)
//
//  Created by leon on 2017/11/7.
//  Copyright © 2017年 leon. All rights reserved.
//
/**
 int   socket(int family, int type, int protocol);
 */
#include <stdio.h>
#include "unp.h"

int main(int argc, const char * argv[]) {
    int listenfd,connfd;
    pid_t childPid;
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //INADDR_ANY 通配地址
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);//自定义 9877
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);//1024
    for (; ; ) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
        //Fork 会返回两次：父进程中返回一次(返回子进程ID号)，子进程中返回一次(返回0)
        if ((childPid = Fork()) == 0){//fork出子进程服务客户
            //process the request
            Close(listenfd);
        }
        Close(connfd);
    }
    return 0;
}
