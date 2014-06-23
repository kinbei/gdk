#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <basictype.h>
#include <util/refcount.h>
#include <net/netpublic.h>

/**
 * Socket 抽象层, 方便 tcp / udp 替换
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
	 * 创建socket, 成功返回0, 失败返回非0
	 */
	virtual int32 create() = 0;

	/**
	 * 关闭socket
	 */
	virtual void close() = 0;

	/**
	 * 关闭socket操作
	 */
	virtual int32 shutdown()
	{
		return ::shutdown( getHandle(), SHUT_RDWR );
	}

	/**
	 * 发送数据操作
	 */
	virtual int32 send( const char* lpBuf, uint32 nBufLen ) = 0;

	/**
	 * 接收数据操作
	 */
	virtual uint32 recv( const char* lpBuf, uint32 nBufLen ) = 0;

protected:
	// 
	SOCKET m_Socket;
};
typedef TRefCountToObj<ISocket> ISocketPtr;

#endif
