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

[使用select函数循环读键盘输入](https://github.com/liuchenjane/-net_IO/blob/master/keyboard.cpp) <br>
[使用select函数循环读键盘输入,select中设置超时时间](https://github.com/liuchenjane/-net_IO/blob/master/keyboard1.cpp)<br>
[用select提高服务器的处理能力](https://github.com/liuchenjane/-net_IO/blob/master/server.cpp)<br>
[相应的客户端](https://github.com/liuchenjane/-net_IO/blob/master/client.cpp)<br>

## 2.poll函数  <br>

```c
#include <poll.h>
int poll(struct pollfd * fds, unsigned int fds, int timeout);
```
<br>

[使用poll函数的TCP服务器](https://github.com/liuchenjane/-net_IO/blob/master/poll_tcp_ip/server.cpp)<br>
[使用poll函数的TCP客户端](https://github.com/liuchenjane/-net_IO/blob/master/poll_tcp_ip/client.cpp)<br>
