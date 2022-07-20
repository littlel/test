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
      * ����tcp�����׽���
      * @param port �����ı��ض˿�
      * @param localIp �󶨵ı�������ip
      * @param backLog accept�жӳ���
      * @return -1����ʧ�ܣ�����Ϊsocket fd��
      */
    static int listen(const uint16_t port, const char* localIp = "0.0.0.0", int backLog = 1024);

    /**
    * ��socket fd��ĳ�������Ͷ˿�
    * @param sock socket fd��
    * @param localIp �󶨵ı�������ip
    * @param port �󶨵ı��ض˿�
    * @return 0����ɹ���-1Ϊʧ��
    */
    static int bindSock(int sock, const char* localIp, uint16_t port);

    /**
    * ����TCP_NODELAY������TCP������ʱ
    * @param sock socket fd��
    * @param on �Ƿ���
    * @return 0����ɹ���-1Ϊʧ��
    */
    static int setNoDelay(int sock, bool on = true);
    
    /**
     * ���ö�дsocket�Ƿ�����
     * @param sock socket fd��
     * @param noblock �Ƿ�����
     * @return 0����ɹ���-1Ϊʧ��
     */
    static int setNoBlocked(int sock, bool noblock = true);
   
    /**
     * ����socket���ջ��棬Ĭ��ò��8K���ң�һ������������
     * ����ͨ�������ں������ļ�����
     * @param sock socket fd��
     * @param size ���ջ����С
     * @return 0����ɹ���-1Ϊʧ��
     */
    static int setRecvBuf(int sock, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
     * ����socket���ջ��棬Ĭ��ò��8K���ң�һ������������
     * ����ͨ�������ں������ļ�����
     * @param sock socket fd��
     * @param size ���ջ����С
     * @return 0����ɹ���-1Ϊʧ��
     */
    static int setSendBuf(int sock, int size = SOCKET_DEFAULT_BUF_SIZE);

    /**
    * ���ú����ɰ󶨸��ö˿�(����TIME_WAITE״̬)
    * @param sock socket fd��
    * @param on �Ƿ���������
    * @return 0����ɹ���-1Ϊʧ��
    */
    static int setReuseable(int sock, bool on = true);

    /**
    * ���ú����ɰ󶨸��ö˿�
    * @param sock socket fd��
    * @param on �Ƿ���������
    * @return 0����ɹ���-1Ϊʧ��
    */
    static int setReusePort(int sock, bool on = true);


    /**
     * ����SO_LINGER����
     * @param sock socket fd��
     * @param second �ں˵ȴ��ر�socket��ʱʱ�䣬��λ��
     * @return 0����ɹ���-1Ϊʧ��
     */
    static int setCloseWait(int sock, int second = 0);


    /**
   * �̰߳�ȫ��in_addrתip�ַ���
   */
    static string inet_ntoa(struct in_addr& addr);

};