//
//  main.c
//
//  Created by leon on 2017/11/7.
//  Copyright © 2017年 leon. All rights reserved.
//
/**
 int   socket(int family, int type, int protocol);
 */
#include <stdio.h>
#include "unp.h"
#include "lh.h"


void tcpserver();
void tcpclient();
//  TCP 并发服务器程序（参照4.7， 4.8，5.2节）
int main(int argc, const char * argv[]) {
    tcpserver(); 
    return 0;
}

void
tcpserver(){
    int listenfd,connfd;
    pid_t childPid;
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //htonl是在地址为32位时可用，若要IPV4和IPV6都可用则使用inet_pton
    //INADDR_ANY 通配地址 系统随机分配,一般为0.0.0.0
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);//自定义 9877
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);//1024
    for (; ; ) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
        //Fork 会返回两次：父进程中返回一次(返回子进程ID号)，子进程中返回一次(返回0)
        // 父进程调用fork()后，子线程分享父进程fork之前的所有描述符(是一个副本)
        if ((childPid = Fork()) == 0){//fork出子进程维护connfd 而父进程则关闭
            Close(listenfd);//子进程 共享父进程资源，listen是父进程的事，子进程则关闭监听
            //process the request ...
            str_echo(connfd);
        }
        Close(connfd);//父进程关闭 已连接的客户套接字 close把引用计数减1，变0时才关闭套接字
    }

}
void
tcpclient(){
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, INADDR_ANY, &servaddr.sin_addr);
    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
    lh_cli_str_echo(stdin, sockfd);
    exit(0);
}

