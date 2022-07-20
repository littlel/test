#include "sockutil.h"
#include <unistd.h>

int SockUtil::listen(const uint16_t port, const char* localIp /*= "0.0.0.0"*/, int backLog /*= 1024*/)
{
	int sockfd = -1;
	if ((sockfd = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
	//	printf("�����׽���ʧ��:%s\n" , get_uv_errmsg(true));
		return -1;
	}
	setReuseable(sockfd);
	setReusePort(sockfd);	
	setNoBlocked(sockfd);
	//���ջ�����1MB
	setRecvBuf(sockfd);
	setSendBuf(sockfd);

	if (bindSock(sockfd, localIp, port) == -1) {
		close(sockfd);
		return -1;
	}

	//��ʼ����
	if (::listen(sockfd, backLog) == -1) {
		//WarnL << "��ʼ����ʧ��:" << get_uv_errmsg(true);
		close(sockfd);
		return -1;
	}

	return sockfd;
}

int SockUtil::bindSock(int sockFd, const char* ifr_ip, uint16_t port)
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ifr_ip,&servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	//�󶨼���
	if (::bind(sockFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		//WarnL << "���׽���ʧ��:" << get_uv_errmsg(true);
		return -1;
	}
	return 0;
}

int SockUtil::setNoDelay(int sockFd, bool on )
{
	int opt = on ? 1 : 0;
	int ret = setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, static_cast<socklen_t>(sizeof(opt)));
	if (ret == -1) {
		printf("set NoDelay failed!");
	}
	return ret;
}

int SockUtil::setNoBlocked(int sock, bool noblock /*= true*/)
{
#if defined(_WIN32)
	unsigned long ul = noblock;
#else
	int ul = noblock;
#endif //defined(_WIN32)
	int ret = ioctl(sock, FIONBIO, &ul); //����Ϊ������ģʽ
	if (ret == -1) {
		printf("���÷�����ʧ��!");
	}
	return ret;
}

int SockUtil::setRecvBuf(int sock, int size /*= SOCKET_DEFAULT_BUF_SIZE*/)
{
	int ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size));
	if (ret == -1) {
		printf("���ý��ջ�����ʧ��!");
	}
	return ret;
}

int SockUtil::setSendBuf(int sock, int size /*= SOCKET_DEFAULT_BUF_SIZE*/)
{
	int ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size));
	if (ret == -1) {
		printf("���÷��ͻ�����ʧ��!");
	}
	return ret;
}

int SockUtil::setReuseable(int sockFd, bool on /*= true*/)
{
	int opt = on ? 1 : 0;
	int ret = setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, static_cast<socklen_t>(sizeof(opt)));
	if (ret == -1) {
		printf("���� SO_REUSEADDR ʧ��!");
	}
	return ret;
}

int SockUtil::setReusePort(int sock, bool on /*= true*/)
{
	int opt = on ? 1 : 0;
	/*set SO_REUSEPORT*/
	int iret = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, static_cast<socklen_t>(sizeof(opt)));
	if (iret == -1) 
	{
		printf("���� SO_REUSEPORT ʧ��!");
	}
	return iret;
}

int SockUtil::setCloseWait(int sockFd, int second /*= 0*/)
{
	linger m_sLinger;
	//�ڵ���closesocket()ʱ��������δ�����꣬����ȴ�
	// ��m_sLinger.l_onoff=0;�����closesocket()��ǿ�ƹر�
	m_sLinger.l_onoff = (second > 0);
	m_sLinger.l_linger = second; //���õȴ�ʱ��Ϊx��
	int ret = setsockopt(sockFd, SOL_SOCKET, SO_LINGER, (char*)&m_sLinger, sizeof(linger));
	if (ret == -1) {
#ifndef _WIN32
		printf( "���� SO_LINGER ʧ��!");
#endif
	}
	return ret;
}

std::string SockUtil::inet_ntoa(struct in_addr& addr)
{
	char buf[20];
	unsigned char* p = (unsigned char*)&(addr);
	snprintf(buf, sizeof(buf), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
	return buf;
}
