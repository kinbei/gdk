#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <basictype.h>
#include <util/refcount.h>
#include <net/netpublic.h>

/**
 * Socket �����, ���� tcp / udp �滻
 */
class ISocket
{
public:
	/**
	 * 
	 */
	ISocket()
	{
		m_Socket = INVALID_SOCKET;
	}

	/**
	 *
	 */
	virtual ~ISocket()
	{

	}

	/**
	 * 
	 */
	SOCKET getHandle() const
	{
		return m_Socket;
	}

	/**
	 * ����socket, �ɹ�����0, ʧ�ܷ��ط�0
	 */
	virtual int32 create() = 0;

	/**
	 * �ر�socket
	 */
	virtual void close() = 0;

	/**
	 * �ر�socket����
	 */
	virtual int32 shutdown()
	{
		return ::shutdown( getHandle(), SHUT_RDWR );
	}

	/**
	 * �������ݲ���
	 */
	virtual int32 send( const char* lpBuf, uint32 nBufLen ) = 0;

	/**
	 * �������ݲ���
	 */
	virtual uint32 recv( const char* lpBuf, uint32 nBufLen ) = 0;

protected:
	// 
	SOCKET m_Socket;
};
typedef TRefCountToObj<ISocket> ISocketPtr;

#endif
