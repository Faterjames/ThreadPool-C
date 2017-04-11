//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<errno.h>
//#include<sys/types.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include <unistd.h>
//
//#define MAX_Line 4096
//
//void ClientInfoManage(buffer)
//{
//    if (isLogInInfo(buffer)) {
//        if (isClientIn) {
//            changeClientState();
//        }
//        sendBackLoginRightInfo();
//    }
//    if (isSendToAnotherInfo()){
//        Msg = getInfo();
//        Clinet_sock = getSendToWhichClient();
//        sendMsgToClient();
//    }
//    else{
//        sendBackErrorInfo();
//    }
//}
//
//int main(){
//    int    listenfd, connfd;
//    struct sockaddr_in     servaddr;
//    char    buff[4096]; //store information
//    int     n;
//
//    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
//        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
//        exit(0);
//    }
//
//    memset(&servaddr, 0, sizeof(servaddr));
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//    servaddr.sin_port = htons(6666);
//
//    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
//        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
//        exit(0);
//    }
//
//    if( listen(listenfd, 10) == -1){
//        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
//        exit(0);
//    }
//
//    printf("======waiting for client's request======\n");
//    while(1){
//        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
//            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
//            continue;
//        }
//        n = recv(connfd, buff, MAX_Line, 0);
//        buff[n] = '\0';
//        printf("recv msg from client: %s\n", buff);
//        close(connfd);
//    }
//
////    close(listenfd);
////    getclient();
////    buffer = getclientInfo();
////    ThreadBool();
////    newThread(ClientInfoManage);
////    newThreadIn(newThread);
//
//
//}
//
//
#include <stdio.h>
#include "ThreadPool.h"


void* func(){
    printf("ss");
}

int main() {
    struct ThreadPool *threadPool = ThreadPool_init(1);

        ThreadPool_work_in(threadPool, (void *) func, NULL);
    while(1);

}