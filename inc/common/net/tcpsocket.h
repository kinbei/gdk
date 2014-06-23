#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <net/netpublic.h>
#include <net/socket.h>
#include <util/refcount.h>
#include <string>

/**
 * Tcp Socket ʵ��
 */
class CTCPSocket : public ISocket
{
public:
	/**
	 *
	 */
	CTCPSocket();

	/**
	 * // explicit �ؼ��ַ�ֹ��������ʽת��
	 */
	explicit CTCPSocket( SOCKET s );

	/**
	 * 
	 */
	virtual ~CTCPSocket();

	/**
	 * ����socket, �ɹ�����0, ʧ�ܷ��ط�0 
	 */
	virtual int32 create();

	/**
	 * �󶨶˿�
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 bind( const struct sockaddr *SockAddr, int nLen );

	/**
	 * �ر�socket
	 */
	virtual void close();

	/**
	 * ���ӷ�����
	 *
	 * \return 0��ʾ�ɹ�, ��0��ʾ������
	 */
	virtual int32 connect( const char *lpstrIP, uint16 nPort );

	/**
	 * �����˿�
	 */
	virtual int32 listen( const std::string& strIP, uint16 nPort, int nBackLog = SOMAXCONN );

	/**
	 * ��������
	 *
	 * \return ���ʧ��, ����NULL, ��NULLʱ��Ҫ delete
	 */
	virtual CTCPSocket* accept( struct sockaddr *lpAddr, socklen_t *nAddrlen );

	/**
	 * �������ݲ���
	 *
	 */
	virtual int32 send( const char* lpBuf, uint32 nBufLen );

	/**
	 * �������ݲ���
	 */
	virtual uint32 recv( const char* lpBuf, uint32 nBufLen );
};
typedef TRefCountToObj<CTCPSocket> CTCPSocketPtr;

#endif
