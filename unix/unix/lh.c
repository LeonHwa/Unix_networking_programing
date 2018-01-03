//
//  lh.c
//  Unix
//
//  Created by leon on 2017/11/10.
//  Copyright © 2017年 leon. All rights reserved.
//

#include "lh.h"
#include "unp.h"


void lh_cli_str_echo(FILE *fp,int sockfd){
    char sendline[MAXLINE],recvline[MAXLINE];
    /** plan1 常规操作
   
    while (Fgets(sendline, MAXLINE, fp)) {
        Write(sockfd, sendline, strlen(sendline));
        if(Readline(sockfd, recvline, MAXLINE) == 0){
            err_quit("服务器断开");
        }
        Fputs(recvline, stdout);
    }
   */
    
    /**  使用select I／O复用
       macOS select 最大描述符个数为1024 定义在 #include <sys/types.h>  FD_SETSIZE
     */

    int maxfd1;//最大描述符的值+1
    int stdineof = 0;
    char buff[MAXLINE];
    int n;
    fd_set rset;
    FD_ZERO(&rset);//初始化fd_set
    for (; ; ) {
        if(stdineof == 0){
            FD_SET(fileno(fp),&rset);
        }
        FD_SET(sockfd,&rset);
        maxfd1 = max(sockfd, sockfd) + 1;
        Select(maxfd1, &rset, NULL, NULL, NULL);//读操作
       /*plan2
//        if (FD_ISSET(sockfd,&rset)){//sockfd 准备好了
//            if( Readline(sockfd, buff, MAXLINE) == 0){
//                    err_quit("服务器断开");
//            }
//            Fputs(buff, stdout);//stdout 输出
//        }
//
//        if (FD_ISSET(fileno(fp),&rset)){//stdin 准备好了
//            if( Fgets(sendline, MAXLINE, fp) == NULL)
//                    return;//all done
//            Write(sockfd, sendline, strlen(sendline));
//        }
          */
        //plan3  改进
        if (FD_ISSET(sockfd,&rset)){//sockfd 准备好了
            if((n= Read(sockfd, buff, MAXLINE)) == 0){
                if(stdineof == 1)
                    return;
                else
                  err_quit("服务器断开");
            }
            Write(fileno(stdout), buff,n);//stdout 输出
        }
        
        if (FD_ISSET(fileno(fp),&rset)){//stdin 准备好了
            if((n= Read(fileno(fp), buff, MAXLINE)) == 0){
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);// send FIN
                FD_CLR(fileno(fp),&rset);//移除 描述符
                continue;
            }
            Write(sockfd, buff, strlen(buff));
        }
    }

}
