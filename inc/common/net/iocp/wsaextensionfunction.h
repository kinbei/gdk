#ifndef _WSA_EXTENSION_FUNCTION_H_
#define _WSA_EXTENSION_FUNCTION_H_

#include <net/netpublic.h>

#ifdef WINDOWS
#include <mswsock.h>

/**
 * 保存WSA扩展函数指针 
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
	 * 函数说明
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
	 * 对AcceptEx的封装
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
	 * 封装ConnectEx函数
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
	 * 封装GetAcceptExSockaddrs函数
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
	 * 获取WSA库的函数指针
	 * 如果在没有事先获得函数指针的情况下直接调用函数(也就是说,编译时静态连接mswsock.lib,在程序中直接调用函数),
	 * 那么性能将很受影响.因为AcceptEx()被置于Winsock2架构之外,每次调用时它都被迫通过WSAIoctl()取得函数指针.
	 * 要避免这种性能损失,需要使用这些API的应用程序应该通过调用WSAIoctl()直接从底层的提供者那里取得函数的指针. 
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
	 * 获取AcceptEx函数指针
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
	 * 获取ConnectEx函数指针
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
	 * 获取 GetAcceptExSockaddrs
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
