#pragma once
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <cstdint>
using namespace std;


#define SOCKET_DEFAULT_BUF_SIZE (1 * 1024 * 1024)


class SockUtil
{
public:
    /**
      * 创建tcp监听套接字
      * @param port 监听的本地端口
      * @param localIp 绑定的本地网卡ip
      * @param backLog accept列队长度
      * @return -1代表失败，其他为socket fd号
      */
    static int listen(const uint16_t port, const char* localIp = "0.0.0.0", int backLog = 1024);

    /**
    * 绑定socket fd至某个网卡和端口
    * @param sock socket fd号
    * @param localIp 绑定的本地网卡ip
    * @param port 绑定的本地端口
    * @return 0代表成功，-1为失败
    */
    static int bindSock(int sock, const char* localIp, uint16_t port);

    /**
    * 开启TCP_NODELAY，降低TCP交互延时
    * @param sock socket fd号
    * @param on 是否开启
    * @return 0代表成功，-1为失败
    */
    static int setNoDelay(int sock, bool on = true);
    
    /**
     * 设置读写socket是否阻塞
     * @param sock socket fd号
     * @param noblock 是否阻塞
     * @return 0代表成功，-1为失败
     */
    static int setNoBlocked(int sock, bool noblock = true);
   
    /**
     * 设置socket接收缓存，默认貌似8K左右，一般有设置上限
     * 可以通过配置内核配置文件调整
     * @param sock socket fd号
     * @param size 接收缓存大小
     * @return 0代表成功，-1为失败
     */
    static int setRecvBuf(int sock, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
     * 设置socket接收缓存，默认貌似8K左右，一般有设置上限
     * 可以通过配置内核配置文件调整
     * @param sock socket fd号
     * @param size 接收缓存大小
     * @return 0代表成功，-1为失败
     */
    static int setSendBuf(int sock, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
    * 设置后续可绑定复用端口(处于TIME_WAITE状态)
    * @param sock socket fd号
    * @param on 是否开启该特性
    * @return 0代表成功，-1为失败
    */
    static int setReuseable(int sock, bool on = true);

    /**
    * 设置后续可绑定复用端口
    * @param sock socket fd号
    * @param on 是否开启该特性
    * @return 0代表成功，-1为失败
    */
    static int setReusePort(int sock, bool on = true);


    /**
     * 开启SO_LINGER特性
     * @param sock socket fd号
     * @param second 内核等待关闭socket超时时间，单位秒
     * @return 0代表成功，-1为失败
     */
    static int setCloseWait(int sock, int second = 0);


    /**
   * 线程安全的in_addr转ip字符串
   */
    static string inet_ntoa(struct in_addr& addr);

};