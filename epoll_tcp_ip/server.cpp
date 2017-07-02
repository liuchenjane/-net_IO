#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#define PORT 6666
#define IPADDRESS "127.0.0.1"
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

/* declare functions*/
int socket_bind(const char * ip,int port);
void do_epoll(int listenfd);
void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd,char *buf);
void handle_accept(int epollfd,int listenfd);
void do_read(int epollfd, int fd, char *buf);
void do_write(int epollfd,int fd,char *buf);
void add_event(int epollfd,int fd,int state);
void modify_event(int epollfd,int fd,int state);
void delete_event(int epollfd,int fd,int state);


int main(int argc,char *argv[]){
    int listenfd=socket_bind(IPADDRESS, PORT);
    listen(listenfd,LISTENQ);
    do_epoll(listenfd);
    return 0;
}
int socket_bind(const char * ip,int port){
    int listenfd;
    struct sockaddr_in servaddr;
    unsigned int sin_size;
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        return -1;
    }
    bzero(&(servaddr),sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
  //  servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port=htons(port);
    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr))==-1){
        perror("bind");
        return -2;
    }
    printf("bind ok. %d\n",listenfd);
    return listenfd;
}

//IO epoll
void do_epoll(int listenfd){
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    memset(buf,0,MAXSIZE);
    epollfd=epoll_create(FDSIZE);
    add_event(epollfd,listenfd,EPOLLIN);
    while(1){
        ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
        handle_events(epollfd,events,ret,listenfd,buf);
    }
    close(epollfd);
}

void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd,char *buf){
    int i;
    int fd;

    for(i = 0;i < num;i++){
        fd = events[i].data.fd;
        if ((fd == listenfd) && (events[i].events & EPOLLIN))
            handle_accept(epollfd,listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd,fd,buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd,fd,buf);
    }
}

void handle_accept(int epollfd,int listenfd){
    int cliafd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen=sizeof(cliaddr);
    cliafd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
    printf("cliafd: %d\n",cliafd);
    if (cliafd == -1)
        perror("accept error:");
    else{
        printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
        add_event(epollfd,cliafd,EPOLLIN);
    }
}

void do_read(int epollfd,int fd,char * buf){
    int nread;
    //printf("begin read.\n");
    nread = read(fd,buf,MAXSIZE);
    if (nread == -1){
        perror("read error:");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }
    else if (nread == 0){
        fprintf(stderr, "close client.\n");
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
    }else{
        printf("read message is: %s\n",buf );
        modify_event(epollfd,fd,EPOLLOUT);
    }
}

void do_write(int epollfd,int fd,char *buf){
    int nwrite;
    nwrite = write(fd,buf,strlen(buf));
    if (nwrite == -1){
        perror("write error:");
        close(fd);
        delete_event(epollfd,fd,EPOLLOUT);
    }
    else{
        modify_event(epollfd,fd,EPOLLIN);
    }
    memset(buf,0,MAXSIZE);
}

void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

void delete_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}
void modify_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd =fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}


