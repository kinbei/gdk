#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <util/refcount.h>
#include <net/connection.h>
#include <net/connectionlistener.h>
#include <net/connectorlistener.h>

/**  
 * 
 */
class CConnector
{
public:
	/**
	 *
	 */
	CConnector() 
	{
		//
		m_pSocket = NULL;
		//
		m_pAddress = NULL;
		//
		m_pListener = NULL;
	}

	/**
	 * 
	 */
	virtual ~CConnector()
	{

	}

	/**
	 * 异步连接
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 connect( const char* lpszIPAddress, uint16 nPort )
	{
		int32 retCode = -1;

		if ( m_pSocket == NULL )
			m_pSocket = new CTCPSocket();

		if ( m_pSocket->getHandle() != INVALID_SOCKET )
			m_pSocket->close();

		retCode = m_pSocket->create();
		if ( retCode != 0 )
			return retCode;

		// 这里必须先绑定, 如果不绑定, 调用 connectex 时会返回错误码
		sockaddr_in SockAddr;
		memset(&SockAddr, 0x00, sizeof(sockaddr_in) ); 
		SockAddr.sin_family = AF_INET;
		retCode = m_pSocket->bind( (const sockaddr*)&SockAddr, sizeof(SockAddr) );
		if ( retCode != 0 )
			return retCode;

		m_pAddress = new CAddress( lpszIPAddress, nPort );

		return 0;
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
			return INVALID_SOCKET;

		return m_pSocket->getHandle();
	}

	/**
	 * 获取地址 
	 *
	 * \param 
	 * \return 
	 */
	CAddressPtr getAddress()
	{
		return m_pAddress;
	}

	/**
	 * 关闭
	 */
	void close()
	{
		m_pSocket->close();
	}

	/**
	 * 设置监听事件
	 *
	 * \param 
	 * \return 
	 */
	void setListener( IConnectorListenerPtr pListener )
	{
		m_pListener = pListener;
	}

	/**
	 * 获取监听事件
	 *
	 * \param 
	 * \return 
	 */
	IConnectorListenerPtr getListener()
	{
		return m_pListener;
	}

private:
	//
	CTCPSocketPtr m_pSocket;
	//
	CAddressPtr m_pAddress;
	//
	IConnectorListenerPtr m_pListener;
};
typedef TRefCountToObj<CConnector> CConnectorPtr;

#endif
