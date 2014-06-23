#ifndef _WSA_EXTENSION_FUNCTION_H_
#define _WSA_EXTENSION_FUNCTION_H_

#include <net/netpublic.h>

#ifdef WINDOWS
#include <mswsock.h>

/**
 * ����WSA��չ����ָ�� 
 */
class CWSAExtensionFunction
{
public:
	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	CWSAExtensionFunction()
	{

	}
	
	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	virtual ~CWSAExtensionFunction()
	{

	}

	/**
	 * ����˵��
	 *
	 * \param 
	 * \return 
	 */
	static int32 init(void)
	{
		m_lpfnAcceptEx = GetAcceptExFunctionPtr();
		if ( m_lpfnAcceptEx == NULL )
			return SOCKET_ERROR;

		m_lpfnConnectEx = GetConnectExFunctionPtr();
		if ( m_lpfnConnectEx == NULL )
			return SOCKET_ERROR;

		m_lpfnGetAcceptExSockaddrs = GetAcceptExSockaddrsFunctionPtr();
		if ( m_lpfnGetAcceptExSockaddrs == NULL )
			return SOCKET_ERROR;

		return 0;
	}

	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	static void uninit()
	{
		m_lpfnAcceptEx = NULL;
		m_lpfnConnectEx = NULL;
		m_lpfnGetAcceptExSockaddrs = NULL;
	}

	/**
	 * ��AcceptEx�ķ�װ
	 *
	 * \param 
	 * \return 
	 */
	static BOOL AcceptEx( SOCKET sListenSocket,
		SOCKET sAcceptSocket,
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		LPDWORD lpdwBytesReceived,
		LPOVERLAPPED lpOverlapped )
	{
		if( m_lpfnAcceptEx == NULL )
			return FALSE;

		return m_lpfnAcceptEx( sListenSocket, 
			sAcceptSocket,
			lpOutputBuffer,
			dwReceiveDataLength,
			dwLocalAddressLength,
			dwRemoteAddressLength,
			lpdwBytesReceived,
			lpOverlapped ) ;
	}

	/**
	 * ��װConnectEx����
	 *
	 * \param 
	 * \return 
	 */
	static BOOL ConnectEx( SOCKET s,
		 const struct sockaddr FAR *name,
		 int namelen,
		 PVOID lpSendBuffer,
		 DWORD dwSendDataLength,
		 LPDWORD lpdwBytesSent,
		 LPOVERLAPPED lpOverlapped )
	{
		if ( m_lpfnConnectEx == NULL )
			return FALSE;

		return m_lpfnConnectEx( s, name, namelen, lpSendBuffer, dwSendDataLength, lpdwBytesSent, lpOverlapped );
	}

	/**
	 * ��װGetAcceptExSockaddrs����
	 *
	 * \param 
	 * \return 
	 */
	static VOID GetAcceptExSockaddrs( PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		struct sockaddr **LocalSockaddr,
		LPINT LocalSockaddrLength,
		struct sockaddr **RemoteSockaddr,
		LPINT RemoteSockaddrLength )
	{
		if ( m_lpfnGetAcceptExSockaddrs == NULL )
			return ;

		m_lpfnGetAcceptExSockaddrs( lpOutputBuffer, 
			dwReceiveDataLength,
			dwLocalAddressLength,
			dwRemoteAddressLength,
			LocalSockaddr,
			LocalSockaddrLength,
			RemoteSockaddr,
			RemoteSockaddrLength );
	}

protected:
	/**
	 * ��ȡWSA��ĺ���ָ��
	 * �����û�����Ȼ�ú���ָ��������ֱ�ӵ��ú���(Ҳ����˵,����ʱ��̬����mswsock.lib,�ڳ�����ֱ�ӵ��ú���),
	 * ��ô���ܽ�����Ӱ��.��ΪAcceptEx()������Winsock2�ܹ�֮��,ÿ�ε���ʱ��������ͨ��WSAIoctl()ȡ�ú���ָ��.
	 * Ҫ��������������ʧ,��Ҫʹ����ЩAPI��Ӧ�ó���Ӧ��ͨ������WSAIoctl()ֱ�Ӵӵײ���ṩ������ȡ�ú�����ָ��. 
	 *
	 * \param 
	 * \return 
	 */
	static PVOID WSAGetExtensionFuncPtr( GUID Guid )
	{
		DWORD dwBytes = 0;
		PVOID lpfn = NULL;

		SOCKET s = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
		if ( s == INVALID_SOCKET )
			return NULL;

		// Load the extension function into memory using WSAIoctl.
		// The WSAIoctl function is an extension of the ioctlsocket()
		// function that can use overlapped I/O. The function's 3rd
		// through 6th parameters are input and output buffers where
		// we pass the pointer to our extension function. This is used
		// so that we can call the extension function directly, rather
		// than refer to the Mswsock.lib library.
		if( ::WSAIoctl(	s, SIO_GET_EXTENSION_FUNCTION_POINTER, &Guid, sizeof(Guid), &lpfn, sizeof(lpfn), &dwBytes, NULL, NULL ) == SOCKET_ERROR )
			return NULL;

		DISABLE_UNREFERENCE( ::closesocket( s ) );

		return lpfn;
	}

	/**
	 * ��ȡAcceptEx����ָ��
	 *
	 * \param 
	 * \return 
	 */
	static inline LPFN_ACCEPTEX GetAcceptExFunctionPtr(void)
	{
		GUID Guid = WSAID_ACCEPTEX;
		return (LPFN_ACCEPTEX)WSAGetExtensionFuncPtr( Guid );
	}

	/**
	 * ��ȡConnectEx����ָ��
	 *
	 * \param 
	 * \return 
	 */
	static inline LPFN_CONNECTEX GetConnectExFunctionPtr(void)
	{
		GUID Guid = WSAID_CONNECTEX;
		return (LPFN_CONNECTEX)WSAGetExtensionFuncPtr( Guid );
	}

	/**
	 * ��ȡ GetAcceptExSockaddrs
	 *
	 * \param 
	 * \return 
	 */
	static inline LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrsFunctionPtr(void)
	{
		GUID Guid = WSAID_GETACCEPTEXSOCKADDRS;
		return (LPFN_GETACCEPTEXSOCKADDRS)WSAGetExtensionFuncPtr( Guid );
	}

private:
	//
	static LPFN_ACCEPTEX m_lpfnAcceptEx;
	//
	static LPFN_CONNECTEX m_lpfnConnectEx;
	//
	static LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs;
};

#endif // WINDOWS

#endif
