#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <util/refcount.h>
#include <net/address.h>
#include <net/socket.h>
#include <net/connectionlistener.h>
#include <io/bytesbuffer.h>
#include <util/debug.h>

#define min(a,b)    (((a) < (b)) ? (a) : (b))

/**  
 * 连接类
 */
class CConnection
{
public:
	/**
	 * 函数说明
	 *
	 * \param 
	 * \return 
	 */
	CConnection( ISocketPtr pSocket, CAddressPtr pAddress )
	{
		//
		m_pSocket = pSocket;
		//
		m_pAddress = pAddress;
		// 事件处理
		m_pListener = NULL;
		// 发送缓冲区
		m_pSendBuffer = new CBytesBuffer();
		// 接收缓冲区
		m_pRecvBuffer = new CBytesBuffer();
	}

	/**
	 * 函数说明
	 *
	 * \param 
	 * \return 
	 */
	virtual ~CConnection()
	{
		//
		m_pSocket = NULL;
		//
		m_pAddress = NULL;
		// 事件处理
		m_pListener = NULL;
		// 发送缓冲区
		m_pSendBuffer = NULL;
		// 接收缓冲区
		m_pRecvBuffer = NULL;
	}

	/**
	 * 发送数据
	 *
	 * \param 
	 * \return 
	 */
	int32 send( const char* lpBuf, uint32 nBufLen )
	{
		if ( m_pSocket == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) Socket is NULL", (void*)this );
			return -1;
		}

		if ( m_pSendBuffer == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) SendBuffer is NULL", (void*)this );
			return -1;
		}

		char *pDestBuf = m_pSendBuffer->writebegin( nBufLen );
		if ( pDestBuf == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) Failed to WriteBegin", (void*)this );
			return -1;
		}

		memcpy( pDestBuf, lpBuf, nBufLen );

		m_pSendBuffer->writecommit();
		return 0;
	}

	/**
	 * 完成发送
	 *
	 * \param 
	 * \return 
	 */
	void finishSend( uint32 nLen )
	{
		if ( m_pSendBuffer == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) SendBuffer is NULL", (void*)this );
			return ;
		}

		if ( m_pAddress == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) Address is NULL", (void*)this );
			return ;
		}

		if ( m_pSendBuffer->getDataSize() == 0 )
		{
			DEBUG_INFO( "Connection(0x%08X) SizeOfSendBuf(%d) SendBytes(%d) Address(%s)", (void*)this, m_pSendBuffer->getDataSize(), nLen, m_pAddress->asString() );
			return ;
		}

		m_pSendBuffer->popBytes( nLen );
	}

	/**
	 * working for epoll only
	 */
	int32 recv()
	{
		if( m_pSocket == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) Socket is null", m_pSocket.get() );
			return -1;
		}

		//TODO modify 1024 as a macro
		char *pDestBuf = m_pRecvBuffer->writebegin( 1024 );
		if ( pDestBuf == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) recv buffer is null", m_pSocket.get() );
			return -1;
		}

		int32 nRetCode = m_pSocket->recv( pDestBuf, 1024 );
		if ( nRetCode > 0 )
			m_pRecvBuffer->writecommit();

		return nRetCode;
	}

	/**
	 * working for epoll only
	 */
	int32 send()
	{
		// send 1024 bytes each time at most
		int32 nlen = min( m_pSendBuffer->getDataSize(), 1024 );

		int32 retcode = m_pSocket->send( m_pSendBuffer->getRowDataPointer(), nlen );

		if ( retcode == 0 )
		{
			m_pSendBuffer->popBytes( nlen );
			return 0;
		}
		else
		{
			return retcode;
		}
	}

	/**
	 * working for epoll only
	 */
	bool needSend() const
	{
		if ( m_pSendBuffer == NULL )
			return false;

		return ( m_pSendBuffer->getDataSize() != 0 );
	}

	/**
	 * working for iocp only
	 */
	void pushRecvData( const char* lpBuf, uint32 nBufLen )
	{
		if ( m_pRecvBuffer == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) SendBuffer is NULL", (void*)this );
			return ;
		}

		char *pDestBuf = m_pRecvBuffer->writebegin( nBufLen );
		if ( pDestBuf == NULL )
		{
			DEBUG_INFO( "Connection(0x%08X) SizeOfRecvBuf(%d) RecvBytes(%d) Address(%s)", (void*)this, m_pRecvBuffer->getDataSize(), nBufLen, m_pAddress->asString() );
			return ;
		}

		memcpy( pDestBuf, lpBuf, nBufLen );
		m_pRecvBuffer->writecommit();
	}

	/**
	 * 关闭连接
	 *
	 * \param 
	 * \return 
	 */
	void close()
	{
		if ( m_pSocket != NULL )
			m_pSocket->close();
	}

	/**
	 * 获取句柄
	 *
	 * \param 
	 * \return 
	 */
	SOCKET getHandle()
	{
		if ( m_pSocket == NULL )
			return 0;
		else
			return m_pSocket->getHandle();
	}

	/**
	 * 获取网络地址
	 *
	 * \param 
	 * \return 
	 */
	CAddressPtr getAddress()
	{
		return m_pAddress;
	}

	/**
	 * 设置事件处理
	 *
	 * \param 
	 * \return 
	 */
	void setListener( IConnectionListenerPtr pListener )
	{
		m_pListener = pListener;
	}

	/**
	 * 获取事件处理
	 *
	 * \param 
	 * \return 
	 */
	IConnectionListenerPtr getListener()
	{
		return m_pListener;
	}

	/**
	 * 
	 */
	CBytesBufferPtr getSendBuffer()
	{
		return m_pSendBuffer;
	}

	/**
	 * 
	 */
	CBytesBufferPtr getRecvBuffer()
	{
		return m_pRecvBuffer;
	}

private:
	// Socket
	ISocketPtr m_pSocket;
	// IP地址
	CAddressPtr m_pAddress;
	// 事件处理
	IConnectionListenerPtr m_pListener;
	// 发送缓冲区
	CBytesBufferPtr m_pSendBuffer;
	// 接收缓冲区
	CBytesBufferPtr m_pRecvBuffer;
};
typedef TRefCountToObj<CConnection> CConnectionPtr;

#endif
