# -net_IO
基本网络IO模型
主要针对多路IO复用模型

## 1. select 模型
select 会不断轮询所负责的所有socket，当某个socket有数据到达了，通知用户进程。<br>
select 接口原型：<br>
```c
FD_ZERO( fd_set * fds);/*清零*/
FD_SET(int fd, fd_set * fds);/*将fd加入fds中*/
FD_ISSET(int fd, fd_set * fds);/*如果fd在fds中，则为真*/
FD_CLR(int fd, fd_set * fds);/*将fd从set中清除*/
int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval *timeout);
//nfds：所有文件描述符的最大值加一；readfds：监视文件描述符的读变化；writefds：监视文件描述符的写变化；
//errorfds：监视文件错误异常；
//timeout：select的超时异常，设为NULL,将select设为阻塞状态；设为0，非阻塞函数；大于0，表示等待的超时时间
```
[用select提高函数的处理能力](https://github.com/liuchenjane/-net_IO/blob/master/server.cpp)
