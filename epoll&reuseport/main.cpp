#include <iostream>
using namespace std;
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <unordered_map>

#include "sockutil.h"

using namespace std;

/*測試内容：
1、测试tcp监听和创建绑定同一个端口，使用reuseport方式
2、tcp和udp绑定同一个端口
3、测试epoll进行网络处理
*/


int _udp;
int _tcp;

int setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void addfd(int epollfd, int fd)
{
	//setnonblocking(fd);
	struct epoll_event event;
	event.data.fd = fd;
	//event.events = EPOLLIN | EPOLLET;
	event.events = EPOLLIN;
	int iret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	int aa;
}

void delfd(int epollfd,int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	//event.events = EPOLLIN | EPOLLET;
	event.events = EPOLLIN;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event);

}

void tcp_accept(int epollfd)
{
	int clifd;
	struct sockaddr_in cliaddr;
	bzero(&cliaddr,sizeof(cliaddr));
	socklen_t cliaddrlen = sizeof(sockaddr_in);
	clifd = accept(_tcp,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if (clifd == -1)
	{
		perror("accept error:%s.\n");
		cout<< strerror(errno)<<endl;
	}
	else
	{
		cout << "accept a new client:" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port)<<endl;
		addfd(epollfd,clifd);
	}
}

void tcp_recv(int epollfd, int clifd)
{
	//epoll-LT模式，不用读尽,会再次通知。解决粘包问题，需要缓存接收的报文，异步解析sip。
	int nread;
	char buf[1024] = { 0 };
	nread = read(clifd, buf, 1024);
	if (nread <= 0)
	{
		close(clifd);
		printf("error:%d->%s.\n",errno,strerror(errno));
		delfd(epollfd, clifd);
	}
	else
	{
		cout << "tcp client buf:" << buf << endl;
	}
}

void udp_recv()
{
	char buf[1024];
	memset(buf, '\0', 1024);
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);

	int ret = recvfrom(_udp, buf, 1024 - 1, 0, (struct sockaddr*)&client_address, &client_addrlength);

	cout << "udp client buf:" << buf << endl;
}

#define EPOLL_SIZE 1024

int main(int argc,char** argv)
{
	if (argc < 2)
	{
		printf("no port\n");
		return 0;
	}
	int port = atoi(argv[1]);
	int iret;
	
	do 
	{
		_tcp = SockUtil::listen(port);
		if (!_tcp)
		{
			cout << "tcp listen failed." << endl;
			break;
		}

		_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		//iret = SockUtil::bindSock(_udp, "192.168.85.139", port);
		iret = SockUtil::bindSock(_udp, "0.0.0.0", port);
		if (iret != 0)
		{
			cout << "bind udp sock failed:" << strerror(errno) << endl;
			break;
		}

		SockUtil::setNoBlocked(_udp);
		SockUtil::setSendBuf(_udp);
		SockUtil::setRecvBuf(_udp);		

		int epfd = epoll_create(1);

		addfd(epfd,_tcp);
		addfd(epfd,_udp);

		uint64_t minDelay = 20;
		struct epoll_event events[EPOLL_SIZE];
		while (1)
		{
			int ret = epoll_wait(epfd, events, EPOLL_SIZE, 60);
			if (ret == 0)
			{
				continue;
			}
			if (ret < 0) {
				//超时或被打断
				if (errno != EINTR)
				{
					cout << "error no:" << errno <<",info:" <<strerror(errno) << endl;
					break;
				}
				continue;
			}
			

			//处理epoll事件
			for (int i = 0; i < ret; i++) {
				struct epoll_event& ev = events[i];
			
				int fd = ev.data.fd;
				if (fd == _tcp)
				{
					tcp_accept(epfd);
				}
				else if (fd == _udp)
				{
					udp_recv();
				}
				else
				{
					tcp_recv(epfd,fd);
				}
			}
		}

	} while (0);
	
	return 0;
}