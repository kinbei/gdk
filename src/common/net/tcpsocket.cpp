#include <net/tcpsocket.h>
#include <string.h>

CTCPSocket::CTCPSocket()
{

}

CTCPSocket::CTCPSocket( SOCKET s )
{
	m_Socket = s;
}

CTCPSocket::~CTCPSocket()
{

}

int32 CTCPSocket::create()
{
	// 如果已经创建 socket, 返回成功
	if ( INVALID_SOCKET != this->m_Socket )
		return 0;

	// AF_INET: The Internet Protocol version 4 (IPv4) address family.
	// SOCK_STREAM: A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. 
	//				This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
	// IPPROTO_TCP: The Transmission Control Protocol (TCP). This is a possible value when the af parameter is AF_INET or AF_INET6 and the type parameter is SOCK_STREAM.

	this->m_Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	
	if ( INVALID_SOCKET == this->m_Socket )
		return GetLastNetError();
	else
		return 0;
}

void CTCPSocket::close()
{
	DISABLE_UNREFERENCE( CloseSocket( m_Socket ) );
}

int32 CTCPSocket::connect( const char *lpstrIP, uint16 nPort )
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( nPort );
	addr.sin_addr.s_addr = inet_addr( lpstrIP );

	// windows: If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, 
	//	and a specific error code can be retrieved by calling WSAGetLastError.

	if( SOCKET_ERROR == ::connect( this->m_Socket, (const struct sockaddr*)&addr, sizeof(sockaddr_in) ) )
		return GetLastNetError();
	else
		return 0;
}

int32 CTCPSocket::listen( const char *lpstrIP, uint16 nPort, int nBackLog )
{
	int nOn = 1;

#ifdef WINDOWS
	if ( -1 == setsockopt( this->m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOn, sizeof(nOn)) )
		return GetLastNetError();
#else
	if ( -1 == setsockopt( this->m_Socket, SOL_SOCKET, SO_REUSEADDR, &nOn, sizeof(nOn)) )
		return GetLastNetError();
#endif

	struct sockaddr_in addr;
	memset( &addr, 0x00, sizeof(struct sockaddr_in) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( lpstrIP );
	addr.sin_port = htons( (short)(nPort) );

	// windows: If no error occurs, bind returns zero. 
	//			Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	// unix: On success, zero is returned. On error, -1 is returned, and errno is set appropriately. 
	if( SOCKET_ERROR == ::bind( this->m_Socket, (struct sockaddr*)&addr, sizeof(struct sockaddr)) )
		return GetLastNetError();

	//TODO listen 这里需要设置接收缓冲区?
	// 设置接收缓冲区
// 	int nRecvSize = 1024*32;
// 	if ( -1 == setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&nRecvSize, sizeof(int) ) )
// 		return GetLastNetError();

	// 监听端口
	if( -1 == ::listen( m_Socket, nBackLog ) )
		return GetLastNetError();
	else
		return 0;
}

CTCPSocket* CTCPSocket::accept( struct sockaddr *lpAddr, socklen_t *nAddrlen )
{
	SOCKET RemoteSocket = ::accept( this->m_Socket, lpAddr, nAddrlen );
	if ( INVALID_SOCKET == RemoteSocket )
		return NULL;
	else
		return new CTCPSocket( RemoteSocket );
}

int32 CTCPSocket::send( const char* lpBuf, uint32 nBufLen )
{
	return ::send( this->m_Socket, lpBuf, nBufLen, 0 );
}

uint32 CTCPSocket::recv( const char* lpBuf, uint32 nBufLen )
{
	return ::recv( this->m_Socket, (char*)lpBuf, nBufLen, 0 );
}

int32 CTCPSocket::bind( const struct sockaddr *SockAddr, int nLen )
{
	return ::bind( this->m_Socket, SockAddr, nLen );
}
