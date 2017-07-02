#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <sys/wait.h>
#include <poll.h>
#define MAXLINE 1024
#define DEFAULT_PORT 6666
#define max(a,b) (a>b)?a:b
static void handle_connection(int sockfd);
int main(int argc, char ** argv){
    int connfd=0,n;
	char recvline[4096],sendline[4096];
	struct sockaddr_in client;

	if(argc != 2){
		printf("usage: ./client <ipaddress>\n");
		return 0;
	}

	if( (connfd=socket(AF_INET,SOCK_STREAM,0))<0 ){
		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		return 0;
	}

	memset(&client,0,sizeof(client));
	client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr(argv[1]);
	client.sin_port=htons(6666);

	if( connect(connfd, (struct sockaddr*)&client, sizeof(client))<0 ){
        perror("connect");
		return -1;
	}
    /*handle connfd*/
    handle_connection(connfd);
	close(connfd);
	return 0;
}
static void handle_connection(int sockfd){
    char sendline[MAXLINE],recvline[MAXLINE];
    int maxfdp,stdineof;
    struct pollfd pfds[2];
    int n;
    /*add connfd*/
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;
    /*add STDIN*/
    pfds[1].fd = STDIN_FILENO;
    pfds[1].events = POLLIN;
    while(1){
        poll(pfds,2,-1);
        if (pfds[0].revents & POLLIN){
            n = read(sockfd,recvline, MAXLINE);
            if (n == 0){
                fprintf(stderr,"client: server is closed.\n");
                close(sockfd);
            }
            write(STDOUT_FILENO,recvline,n);
        }
        /*test STDIN is ok?*/
        if (pfds[1].revents & POLLIN){
            n = read(STDIN_FILENO, sendline, MAXLINE);
            if (n == 0){
                shutdown(sockfd,SHUT_WR);
                continue;
            }
            write(sockfd,sendline,n);
        }
    }
}
