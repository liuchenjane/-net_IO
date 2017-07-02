#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 6666

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
    char buffer[1024];
    bzero(buffer, sizeof(buffer));
    recv(connfd, buffer, 1024, 0);
    printf("recv: %s\n", buffer);
    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "this is client!\n");
    send(connfd, buffer, 1024,0);
    while(1){
        bzero(buffer,sizeof(buffer));
        scanf("%s",buffer);
        int p=strlen(buffer);
        buffer[p]='\0';
        send(connfd,buffer, 1024,0);
        printf(" i have send buffer\n");
    }

	close(connfd);
	return 0;
}
