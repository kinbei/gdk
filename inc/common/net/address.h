#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <util/refcount.h>
#include <basictype.h>
#include <memory.h>

/**  
 * 网络地址
 */

class CAddress
{
public:
	/**
	 *
	 */
	CAddress( sockaddr_in* pSockAddr )
	{
		// 地址信息
		memcpy( &m_Address, pSockAddr, sizeof(sockaddr_in) );

		// IP
		snprintf( m_szIP, sizeof(m_szIP) - 1, "%s", inet_ntoa( pSockAddr->sin_addr ) );
		m_szIP[ sizeof(m_szIP) - 1 ] = 0;

		// 端口
		m_nPort = ntohs( pSockAddr->sin_port );

		// IP+端口
		snprintf( m_szIPAndPort, sizeof(m_szIPAndPort), "%s:%u", m_szIP, m_nPort );
		m_szIPAndPort[ sizeof(m_szIPAndPort) - 1 ] = '\0';
	}

	/**
	 *
	 */
	CAddress( const char* lpIP, uint16 nPort )
	{
		// 地址信息
		memset( &m_Address, 0x00, sizeof(m_Address) );
		m_Address.sin_family = AF_INET;
		m_Address.sin_port = htons( (short)nPort );
		m_Address.sin_addr.s_addr = inet_addr( lpIP );

		// IP
		snprintf( m_szIP, sizeof(m_szIP) - 1, "%s", lpIP );
		m_szIP[ sizeof(m_szIP) - 1 ] = 0;

		// 端口
		m_nPort = nPort;

		// IP+端口
		snprintf( m_szIPAndPort, sizeof(m_szIPAndPort), "%s:%u", m_szIP, m_nPort );
		m_szIPAndPort[ sizeof(m_szIPAndPort) - 1 ] = '\0';
	}

	/**
	 *
	 */
	virtual ~CAddress()
	{

	}

	/**
	 * 获取IP地址
	 *
	 * \param 
	 * \return 
	 */
	const char* getIP() const
	{
		return m_szIP;
	}

	/**
	 * 获取端口
	 *
	 * \param 
	 * \return 
	 */
	uint16 getPort() const
	{
		return m_nPort;
	}

	/**
	 * 获取IP+端口
	 *
	 * \param 
	 * \return 
	 */
	const char* asString() const
	{
		return m_szIPAndPort;
	}

	/**
	 * 获取地址信息
	 *
	 * \param 
	 * \return 
	 */
	sockaddr_in* getSockAddr()
	{
		return &m_Address;
	}


private:
	/**
	 * 默认构造函数不允许直接调用
	 *
	 * \param 
	 * \return 
	 */
	CAddress(  )
	{

	}

	// 地址信息
	struct sockaddr_in m_Address;
	// 缓存IP地址
	char m_szIP[50];
	// 端口
	uint16 m_nPort;
	// IP和端口
	char m_szIPAndPort[50];
};
typedef TRefCountToObj<CAddress> CAddressPtr;

#endif
