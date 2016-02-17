#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <util/atom.h>
#include <util/mutex.h>
#include <net/socket.h>
#include <util/debug.h>
#include <net/address.h>
#include <util/refcount.h>
#include <io/bytesbuffer.h>
#include <net/connectionlistener.h>


/**  
 * 连接类
 */
class CConnection
{
public:
	/**
	 *
	 */
	CConnection( ISocketPtr pSocket, CAddressPtr pAddress )
	{
		//
		m_pSocket = pSocket;
		//
		m_pAddress = pAddress;
		// 
		m_pListener = NULL;
	}

	/**
	 *
	 */
	virtual ~CConnection()
	{
		//
		m_pSocket = NULL;
		//
		m_pAddress = NULL;
		// 
		m_pListener = NULL;
	}

	/**
	 * send data
	 *
	 * \param 
	 * \return 
	 */
	int32 send( const char* lpBuf, uint32 nBufLen )
	{
		if ( m_pSocket == NULL )
		{
			log_warning( "Connection(%p) Socket is NULL", this );
			return -1;
		}

		return m_pSocket->send( lpBuf, nBufLen );
	}

	/**
	 * recv data
	 * \param
	 * \return
	 */
	int32 recv( const char* lpBuf, uint32 nBufLen )
	{
		if ( m_pSocket == NULL )
		{
			log_warning( "Connection(%p) Socket is NULL", this );
			return -1;
		}

		return m_pSocket->recv( lpBuf, nBufLen );
	}

	/**
	 *
	 */
	void close()
	{
		if ( m_pSocket != NULL )
			m_pSocket->close();
	}

	/**
	 *
	 */
	SOCKET getHandle()
	{
		if ( m_pSocket == NULL )
			return 0;
		else
			return m_pSocket->getHandle();
	}

	/**
	 *
	 */
	CAddressPtr getAddress()
	{
		return m_pAddress;
	}

	/**
	 *
	 */
	void setListener( IConnectionListenerPtr pListener )
	{
		m_pListener = pListener;
	}

	/**
	 *
	 */
	IConnectionListenerPtr getListener()
	{
		return m_pListener;
	}

private:
	// Socket
	ISocketPtr m_pSocket;
	// IP地址
	CAddressPtr m_pAddress;
	// 事件处理
	IConnectionListenerPtr m_pListener;
};
typedef TRefCountToObj<CConnection> CConnectionPtr;

#endif
