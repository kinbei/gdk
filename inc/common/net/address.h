#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <util/refcount.h>
#include <basictype.h>
#include <memory.h>

/**  
 * �����ַ
 */

class CAddress
{
public:
	/**
	 *
	 */
	CAddress( sockaddr_in* pSockAddr )
	{
		// ��ַ��Ϣ
		memcpy( &m_Address, pSockAddr, sizeof(sockaddr_in) );

		// IP
		snprintf( m_szIP, sizeof(m_szIP) - 1, "%s", inet_ntoa( pSockAddr->sin_addr ) );
		m_szIP[ sizeof(m_szIP) - 1 ] = 0;

		// �˿�
		m_nPort = ntohs( pSockAddr->sin_port );

		// IP+�˿�
		snprintf( m_szIPAndPort, sizeof(m_szIPAndPort), "%s:%u", m_szIP, m_nPort );
		m_szIPAndPort[ sizeof(m_szIPAndPort) - 1 ] = '\0';
	}

	/**
	 *
	 */
	CAddress( const char* lpIP, uint16 nPort )
	{
		// ��ַ��Ϣ
		memset( &m_Address, 0x00, sizeof(m_Address) );
		m_Address.sin_family = AF_INET;
		m_Address.sin_port = htons( (short)nPort );
		m_Address.sin_addr.s_addr = inet_addr( lpIP );

		// IP
		snprintf( m_szIP, sizeof(m_szIP) - 1, "%s", lpIP );
		m_szIP[ sizeof(m_szIP) - 1 ] = 0;

		// �˿�
		m_nPort = nPort;

		// IP+�˿�
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
	 * ��ȡIP��ַ
	 *
	 * \param 
	 * \return 
	 */
	const char* getIP() const
	{
		return m_szIP;
	}

	/**
	 * ��ȡ�˿�
	 *
	 * \param 
	 * \return 
	 */
	uint16 getPort() const
	{
		return m_nPort;
	}

	/**
	 * ��ȡIP+�˿�
	 *
	 * \param 
	 * \return 
	 */
	const char* asString() const
	{
		return m_szIPAndPort;
	}

	/**
	 * ��ȡ��ַ��Ϣ
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
	 * Ĭ�Ϲ��캯��������ֱ�ӵ���
	 *
	 * \param 
	 * \return 
	 */
	CAddress(  )
	{

	}

	// ��ַ��Ϣ
	struct sockaddr_in m_Address;
	// ����IP��ַ
	char m_szIP[50];
	// �˿�
	uint16 m_nPort;
	// IP�Ͷ˿�
	char m_szIPAndPort[50];
};
typedef TRefCountToObj<CAddress> CAddressPtr;

#endif
