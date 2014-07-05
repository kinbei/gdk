#include <net/iocp/iocpnetiowrappers.h>
#include <util/thread.h>
#include <util/scopeptr.h>
#include <net/socketlibloader.h>


#ifdef WINDOWS

CIocpNetIoWrappers::CIocpNetIoWrappers()
{
	//
	m_hIOCompletionPort = INVALID_HANDLE_VALUE;
}

CIocpNetIoWrappers::~CIocpNetIoWrappers()
{

}

int32 CIocpNetIoWrappers::init()
{
	// initiates use of the Winsock DLL by a process
	int32 nRetCode = CSocketLibLoader::load();
	if ( nRetCode != 0 )
		return nRetCode;

	// Create Completion Port
	m_hIOCompletionPort = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, NULL, 0 );

	// If the function fails, the return value is NULL. To get extended error information, call the GetLastError function.
	if ( m_hIOCompletionPort == NULL )
		return GetLastNetError();

	return CWSAExtensionFunction::init();
}

void CIocpNetIoWrappers::uninit()
{
	DISABLE_UNREFERENCE( ::CloseHandle( m_hIOCompletionPort ) );

	CSocketLibLoader::unload();
}

int32 CIocpNetIoWrappers::addConnector( CConnectorPtr pConnector )
{
	if ( m_hIOCompletionPort == INVALID_HANDLE_VALUE )
		return -1;

	// Associate the connect socket with the completion port
	if( ::CreateIoCompletionPort( (HANDLE)pConnector->getHandle(), m_hIOCompletionPort, (ULONG_PTR)NULL, 0 ) == NULL )
		return GetLastNetError();

	// Post connect request
	return postConnect( pConnector );
}

int32 CIocpNetIoWrappers::addAcceptor( CAcceptorPtr pAcceptor )
{
	//TODO 
	//第一次关联到完成端口成功, 但PostAccept()失败, 即使再次调用addAcceptor()也不会发送数据

	// Associate the listening socket with the completion port
	if( ::CreateIoCompletionPort( (HANDLE)pAcceptor->getHandle(), m_hIOCompletionPort, (ULONG_PTR)NULL, 0 ) == NULL )
		return GetLastNetError();

	if( postAccept( pAcceptor ) != 0 )
		return -1;

	m_vecAcceptor.push_back( pAcceptor );

	return 0;
}

int32 CIocpNetIoWrappers::run( int32 nTimeOutMilliseconds )
{
	HANDLE* hThreads = new HANDLE[getCPUCount() * 2 + 2];

	// create work thread
	for ( uint32 i = 0; i < getCPUCount() * 2 + 2; i++ )
	{
		CThreadPtr pThread = CThreadFactory::createThread();
		int32 nRetCode = pThread->initialize( WorkerThread, (LPVOID)this );
		if ( nRetCode != 0 )
			return nRetCode;
	}

	CThreadFactory::finalize();
	return 0;
}

void CIocpNetIoWrappers::stop()
{
	for ( std::vector<CAcceptorPtr>::iterator iter_t = m_vecAcceptor.begin(); iter_t != m_vecAcceptor.end(); iter_t++ )
		(*iter_t)->close();

	for ( std::vector<CConnectorPtr>::iterator iter_t = m_vecConnector.begin(); iter_t != m_vecConnector.end(); iter_t++ )
		(*iter_t)->close();

	for ( int32 i = 0; i < ( (int32)(getCPUCount() * 2 + 2) ); i++ )
		DISABLE_UNREFERENCE( ::PostQueuedCompletionStatus( m_hIOCompletionPort, 0, 0, NULL ) );
}

uint32 CIocpNetIoWrappers::getCPUCount()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo( &sysInfo );

	return sysInfo.dwNumberOfProcessors;
}

UINT WINAPI CIocpNetIoWrappers::WorkerThread( LPVOID pParam )
{
	CIocpNetIoWrappers* pThis = (CIocpNetIoWrappers*)( pParam );

	while( true )
	{
		DWORD dwNumberOfBytes = 0;
		DWORD dwCompletionKey = 0;

		LPOVERLAPPED lpOverlapped = NULL;

		// Returns nonzero (TRUE) if successful or zero (FALSE) otherwise.
		BOOL bRet = ::GetQueuedCompletionStatus( pThis->m_hIOCompletionPort, &dwNumberOfBytes, &dwCompletionKey, &lpOverlapped, INFINITE );		

		// 完成端口结束
		if ( lpOverlapped == NULL )
			return 0; 

		//!NOTE: 这里需要明确 lpPreIoData 与 lpPreHandleData 的生命周期
		// lpPreHandleData(dwCompletionKey) 则是整个Socket连接期间的生命周期
		// lpPreIoData 单次IO操作的生命周期
		LPBASE_PRE_IO_DATA lpPreIoData = CONTAINING_RECORD( lpOverlapped, BASE_PRE_IO_DATA, m_OverLapped );
		if ( lpPreIoData == NULL )
		{
			log_error( "lpPreIoData is null" );
			continue; 
		}

		switch ( lpPreIoData->m_OperatorType )
		{
		case BASE_PRE_IO_DATA::IOCP_OPERATOR_ACCEPT:
			{
				CScopePtr<ACCEPT_PRE_IO_DATA> pAcceptPreIoData( (LPACCEPT_PRE_IO_DATA)lpPreIoData );
				CAcceptorPtr pAcceptor = pAcceptPreIoData->m_pAcceptor;
				
				if ( !bRet )
				{
					log_warning( "Accept|Error(%d)", GetLastNetError() );
					break;
				}

				// 获取客户端信息
				struct sockaddr_in* lpLocalSockaddr = NULL;
				struct sockaddr_in* lpRemoteSockaddr = NULL;
				int LocalSockaddrLength = 0;
				int RemoteSockaddrLength = 0;
				DWORD dwReceiveDataLength = 0; //!NOTE: AcceptEx函数传入的 dwReceiveDataLength 为0, GetAcceptExSockaddrs 也必须传0

				CWSAExtensionFunction::GetAcceptExSockaddrs( pAcceptPreIoData->m_szAcceptOutputBuffer, 
					dwReceiveDataLength, 
					sizeof(SOCKADDR_IN) + 16,  
					sizeof(SOCKADDR_IN) + 16, 
					(sockaddr**)&lpLocalSockaddr, 
					&LocalSockaddrLength, 
					(sockaddr**)&lpRemoteSockaddr, 
					&RemoteSockaddrLength );

				int32 retCode = -1;

				retCode = pThis->postAccept( pAcceptor );
				if ( retCode != 0 )
					break;

				CConnectionPtr pConnection = new CConnection( new CTCPSocket( pAcceptPreIoData->m_sAcceptSocket ), new CAddress( lpRemoteSockaddr ) );

				// Associate the connection socket with the completion port
				if( ::CreateIoCompletionPort( (HANDLE)pConnection->getHandle(), pThis->m_hIOCompletionPort, (ULONG_PTR)NULL, 0 ) == NULL )
				{
					log_warning( "Accept|Connection(%p) Error(%d) when associate Iocp", pConnection.get(), GetLastNetError() );
					pConnection->close();
					break;
				}

				// 
				pAcceptor->getListener()->onAccept( pConnection );

				if( pThis->postSend( pConnection ) != 0 || pThis->postRecv( pConnection ) != 0 )
				{
					log_warning( "Accept|Connection(%p) Error when postSend or postRecv", pConnection.get() );
					pConnection->close();
					break;
				}

				pAcceptPreIoData->m_pAcceptor = NULL;
			}
			break;

		case BASE_PRE_IO_DATA::IOCP_OPERATOR_CONNECT:
			{
				int32 retCode = -1;
				CScopePtr<CONNECT_PRE_IO_DATA> pConnectPreIoData ( (LPCONNECT_PRE_IO_DATA)lpPreIoData );
				CConnectorPtr pConnector = pConnectPreIoData->m_pConnector;
				IConnectorListenerPtr pListener = pConnector->getListener();
				CConnectionPtr pConnection = new CConnection( new CTCPSocket( pConnector->getHandle() ), pConnector->getAddress() );

				if ( !bRet )
				{
					log_warning( "Connect|Connection(%p) Error(%d)", pConnection.get(), GetLastNetError() );
					break;
				}

				CAutoLock locker( pConnection->getMutex() );

				pListener->onOpen( pConnection );

				if( pThis->postSend( pConnection ) != 0 || pThis->postRecv( pConnection ) != 0 )
				{
					log_warning( "Connect|Connection(%p) Error when postSend or postRecv", pConnection.get() );
					pConnection->close();
					break;
				}

				pConnectPreIoData->m_pConnector = NULL;
			}
			break;

		case BASE_PRE_IO_DATA::IOCP_OPERATOR_RECV:
			{
				CScopePtr<RECV_PRE_IO_DATA> pRecvPreIoData ( (LPRECV_PRE_IO_DATA)lpPreIoData );
				CConnectionPtr pConnection = pRecvPreIoData->m_pConnection;
				IConnectionListenerPtr pListener = pConnection->getListener();
				CAutoLock locker( pConnection->getMutex() );

				if ( !bRet )
				{
					log_debug( "Receive|Error(%d)", GetLastNetError() );
					break;
					
					if ( pListener != NULL )
						pListener->onClose( pConnection );

					pConnection->close();
					break;
				}

				if ( dwNumberOfBytes == 0 )
				{
					log_debug( "Receive|dwNumberOfByte is zero" );
					break;

					if ( pListener != NULL )
						pListener->onClose( pConnection );

					pConnection->close();
					break;
				}

				log_debug( "Receive|Connection(%p) Bytes(%d)", pConnection.get(), dwNumberOfBytes );

				pRecvPreIoData->m_pConnection->pushRecvData( pRecvPreIoData->m_Buffer.buf, dwNumberOfBytes );

				if ( pListener != NULL )
					pListener->onRecvCompleted( pRecvPreIoData->m_pConnection );

				if( pThis->postSend( pConnection ) != 0 || pThis->postRecv( pConnection ) != 0 )
				{
					log_warning( "Receive|Connection(%p) Error when postSend or postRecv", pConnection.get() );
					pConnection->close();
					break;
				}

				pRecvPreIoData->m_pConnection = NULL;
			}
			break;

		case BASE_PRE_IO_DATA::IOCP_OPERATOR_SEND:
			{
				CScopePtr<SEND_PRE_IO_DATA> pSendPreIoData ( (LPSEND_PRE_IO_DATA)lpPreIoData );
				CConnectionPtr pConnection = pSendPreIoData->m_pConnection;
				IConnectionListenerPtr pListener = pConnection->getListener();
				CAutoLock locker( pConnection->getMutex() );

				if( pConnection == NULL )
				{
					log_warning( "Send|Connection is null");
					break;
				}

				if ( !bRet )
				{
					log_debug( "Send|Connection(%p) Error(%d)", pConnection.get(), GetLastNetError() );

					if ( pListener != NULL )
						pListener->onClose( pConnection );
					pConnection->close();

					break;
				}

				log_debug( "Send|Connection(%p) bytes(%d)", pConnection.get(), dwNumberOfBytes );
				pConnection->finishSend( dwNumberOfBytes );

				if ( pListener != NULL )
					pListener->onSendCompleted( pConnection, dwNumberOfBytes );

				if( pThis->postSend( pConnection ) != 0 )
				{
					log_error( "Send|Connection(%p) Error when postSend", pConnection.get() );

					if ( pListener != NULL )
						pListener->onClose( pConnection );
					pConnection->close();

					break;
				}

				pSendPreIoData->m_pConnection = NULL;
			}
			break;

		default:
			{
				log_error( "Undefined operator type(0x%X)", lpPreIoData->m_OperatorType );
			}
			break;
		}
	}// while

	return 0;
}

int32 CIocpNetIoWrappers::postSend( CConnectionPtr pConnection )
{
	if ( pConnection == NULL )
	{
		log_error( "Connection is null" );
		return -1;
	}

	CBytesBufferPtr pSendBuf = pConnection->getSendBuffer();
	if ( pSendBuf == NULL )
	{
		log_error( "Connection(%p) Send Buffer is null", pConnection );
		return -1;
	}

	// No data transmission return success
	if ( pSendBuf->getDataSize() == 0 )
		return 0;

	if ( pSendBuf->getRowDataPointer() == NULL )
	{
		log_error( "Connection(%p) RowDataPointer of send Buffer is null", pConnection );
		return -1;
	}

	int32 nRetCode = -1;

	//TODO preIoData may get from a object pool
	CScopePtr<SEND_PRE_IO_DATA> preIoData ( new SEND_PRE_IO_DATA() );
	preIoData->m_BasePreIoData.m_OperatorType = BASE_PRE_IO_DATA::IOCP_OPERATOR_SEND;
	preIoData->m_pConnection = pConnection;
	
	int32 nSendLen = min( pSendBuf->getDataSize(), sizeof(preIoData->szBuf) );

	log_debug( "Connection(%p) send %d bytes", pConnection.get(), nSendLen );

	// s [in]
	// A descriptor that identifies a connected socket.

	// lpBuffers [in]
	// A pointer to an array of WSABUF structures. Each WSABUF structure contains a pointer to a buffer and the length, 
	// in bytes, of the buffer. For a Winsock application, once the WSASend function is called, 
	// the system owns these buffers and the application may not access them. This array must remain valid for the duration of the send operation.
	preIoData->m_Buffer.buf = preIoData->szBuf;

	memcpy( preIoData->m_Buffer.buf, pSendBuf->getRowDataPointer(), nSendLen );
	preIoData->m_Buffer.len = nSendLen;

	// dwBufferCount [in]
	// The number of WSABUF structures in the lpBuffers array.

	// lpNumberOfBytesSent [out]
	// A pointer to the number, in bytes, sent by this call if the I/O operation completes immediately. 
	// Use NULL for this parameter if the lpOverlapped parameter is not NULL to avoid potentially erroneous results. 
	// This parameter can be NULL only if the lpOverlapped parameter is not NULL.
	preIoData->m_dwNumberOfBytesSent;

	// dwFlags [in]
	// The flags used to modify the behavior of the WSASend function call. For more information, see Using dwFlags in the Remarks section.
	DWORD dwFlags = 0;

	// lpOverlapped [in]
	// A pointer to a WSAOVERLAPPED structure. This parameter is ignored for nonoverlapped sockets.
	memset(&preIoData->m_BasePreIoData.m_OverLapped, 0x00, sizeof (preIoData->m_BasePreIoData.m_OverLapped));

	// lpCompletionRoutine [in]
	// A pointer to the completion routine called when the send operation has been completed. This parameter is ignored for nonoverlapped sockets.

	if( ::WSASend( pConnection->getHandle(), &preIoData->m_Buffer, 1, NULL, dwFlags, &preIoData->m_BasePreIoData.m_OverLapped, NULL ) != 0 )
	{
		nRetCode = GetLastNetError();
		if ( nRetCode != WSA_IO_PENDING )
			return nRetCode;
	}

	// don't delete preIoData
	preIoData.incRef(); 
	return 0;
}

int32 CIocpNetIoWrappers::postConnect( CConnectorPtr pConnector )
{
	int32 nRetCode = -1;

	CScopePtr<CONNECT_PRE_IO_DATA> preIoData ( new CONNECT_PRE_IO_DATA() );
	preIoData->m_BasePreIoData.m_OperatorType = BASE_PRE_IO_DATA::IOCP_OPERATOR_CONNECT;
	preIoData->m_pConnector = pConnector;

	// s [in]
	// A descriptor that identifies an unconnected, previously bound socket. See Remarks for more information.

	// name [in] 
	// A pointer to a sockaddr structure that specifies the address to which to connect. 
	// For IPv4, the sockaddr contains AF_INET for the address family, the destination IPv4 address, and the destination port. 
	// For IPv6, the sockaddr structure contains AF_INET6 for the address family, the destination IPv6 address, the destination port, 
	// and may contain additional IPv6 flow and scope-id information.

	// namelen [in] 
	// The length, in bytes, of the sockaddr structure pointed to by the name parameter.

	// lpSendBuffer [in, optional] 
	// A pointer to the buffer to be transferred after a connection is established. This parameter is optional.
	// Note  This parameter does not point to "connect data" that is sent as part of connection establishment. 
	// To send "connect data" using the ConnectEx function, 
	// the setsockopt function must be called on the unconnected socket s with the SO_CONNDATA socket option to set a pointer to a buffer containing the connect data, 
	// and then called with the SO_CONNDATALEN socket option to set the length of the "connect data" in the buffer. 
	// Then the ConnectEx function can be called. As an alternative, 
	// the WSAConnect function can be used if connect data is to be sent as part of connection establishment.

	// dwSendDataLength [in] 
	// The length, in bytes, of data pointed to by the lpSendBuffer parameter. This parameter is ignored when the lpSendBuffer parameter is NULL. 

	// lpdwBytesSent [out] 
	// On successful return, this parameter points to a DWORD value that indicates the number of bytes that were sent after the connection was established. 
	// The bytes sent are from the buffer pointed to by the lpSendBuffer parameter. This parameter is ignored when the lpSendBuffer parameter is NULL.
	preIoData->dwBytesSent = 0;

	// lpOverlapped [in] 
	// An OVERLAPPED structure used to process the request. The lpOverlapped parameter must be specified, and cannot be NULL.
	memset(&preIoData->m_BasePreIoData.m_OverLapped, 0x00, sizeof (preIoData->m_BasePreIoData.m_OverLapped));

	CAddressPtr pAddress = pConnector->getAddress();
	if ( pAddress == NULL )
		return -1;

	// On success, the ConnectEx function returns TRUE. On failure, the function returns FALSE. 
	// Use the WSAGetLastError function to get extended error information. If a call to the WSAGetLastError function returns ERROR_IO_PENDING, 
	// the operation initiated successfully and is in progress. Under such circumstances, the call may still fail when the overlapped operation completes.
	BOOL bResult = CWSAExtensionFunction::ConnectEx( pConnector->getHandle(), (sockaddr*)pAddress->getSockAddr(), sizeof(sockaddr), NULL, 0, &preIoData->dwBytesSent, 
		&preIoData->m_BasePreIoData.m_OverLapped );

	if ( !bResult )
	{
		int32 nRetCode = GetLastNetError();
		if ( nRetCode != WSA_IO_PENDING )
			return nRetCode;
	}

	preIoData.incRef();
	return 0;
}

int32 CIocpNetIoWrappers::postRecv( CConnectionPtr pConnection )
{
	int32 nRetCode = -1;

	CScopePtr<RECV_PRE_IO_DATA> preIoData ( new RECV_PRE_IO_DATA() );
	preIoData->m_BasePreIoData.m_OperatorType = BASE_PRE_IO_DATA::IOCP_OPERATOR_RECV;
	preIoData->m_pConnection = pConnection;

	// s [in]
	// A descriptor identifying a connected socket.

	// lpBuffers [in, out]
	// A pointer to an array of WSABUF structures. Each WSABUF structure contains a pointer to a buffer and the length, in bytes, of the buffer.
	preIoData->m_Buffer.buf = preIoData->szBuf;
	preIoData->m_Buffer.len = sizeof( preIoData->szBuf );

	// dwBufferCount [in]
	// The number of WSABUF structures in the lpBuffers array.

	// lpNumberOfBytesRecvd [out]
	// A pointer to the number, in bytes, of data received by this call if the receive operation completes immediately. 
	// Use NULL for this parameter if the lpOverlapped parameter is not NULL to avoid potentially erroneous results. 
	// This parameter can be NULL only if the lpOverlapped parameter is not NULL.

	// lpFlags [in, out]
	// A pointer to flags used to modify the behavior of the WSARecv function call. For more information, see the Remarks section.

	// lpOverlapped [in]
	// A pointer to a WSAOVERLAPPED structure (ignored for nonoverlapped sockets).
	memset(&preIoData->m_BasePreIoData.m_OverLapped, 0x00, sizeof (preIoData->m_BasePreIoData.m_OverLapped));

	preIoData->m_dwFlags = 0;

	if( ::WSARecv( pConnection->getHandle(), &preIoData->m_Buffer, 1, NULL, &preIoData->m_dwFlags, &preIoData->m_BasePreIoData.m_OverLapped, NULL ) != 0 )
	{
		nRetCode = GetLastNetError();
		if ( nRetCode != WSA_IO_PENDING )
			return nRetCode;
	}

	preIoData.incRef();
	return 0;
}

int32 CIocpNetIoWrappers::postAccept( CAcceptorPtr pAcceptor )
{
	int32 nRetCode = -1;

	CScopePtr<ACCEPT_PRE_IO_DATA> preIoData ( new ACCEPT_PRE_IO_DATA() );
	preIoData->m_BasePreIoData.m_OperatorType = BASE_PRE_IO_DATA::IOCP_OPERATOR_ACCEPT;
	preIoData->m_pAcceptor = pAcceptor;

	// The AcceptEx function accepts a new connection, 
	// returns the local and remote address, and receives the first block of data sent by the client application. 

	// sListenSocket [in]
	// A descriptor identifying a socket that has already been called with the listen function. A server application waits for attempts to connect on this socket.
	pAcceptor->getHandle();

	//TODO 这里可以建立一个socket库
	// sAcceptSocket [in]
	// A descriptor identifying a socket on which to accept an incoming connection. This socket must not be bound or connected.
	SOCKET sAcceptSock = ::WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if ( sAcceptSock == INVALID_SOCKET )
		return GetLastNetError();

	preIoData->m_sAcceptSocket = sAcceptSock;

	// lpOutputBuffer [in]
	// A pointer to a buffer that receives the first block of data sent on a new connection, 
	// the local address of the server, and the remote address of the client. 
	// The receive data is written to the first part of the buffer starting at offset zero, 
	// while the addresses are written to the latter part of the buffer. This parameter must be specified.
	// 注意这里的 lpOutputBuffer 不能是本地变量, 如果为本地变量, 在调用 GetQueuedCompletionStatus() 返回后可能造成崩溃
	preIoData->m_szAcceptOutputBuffer;
	memset( preIoData->m_szAcceptOutputBuffer, 0x00, sizeof(preIoData->m_szAcceptOutputBuffer) );

	// dwReceiveDataLength [in]
	// The number of bytes in lpOutputBuffer that will be used for actual receive data at the beginning of the buffer. 
	// This size should not include the size of the local address of the server, nor the remote address of the client; 
	// they are appended to the output buffer. If dwReceiveDataLength is zero, 
	// accepting the connection will not result in a receive operation. Instead, 
	// AcceptEx completes as soon as a connection arrives, without waiting for any data.
	DWORD dwReceiveDataLength = 0;

	// dwLocalAddressLength [in]
	// The number of bytes reserved for the local address information. 
	// This value must be at least 16 bytes more than the maximum address length for the transport protocol in use.
	sizeof(SOCKADDR_IN) + 16;

	// dwRemoteAddressLength [in]
	// The number of bytes reserved for the remote address information. 
	// This value must be at least 16 bytes more than the maximum address length for the transport protocol in use. Cannot be zero.
	sizeof(SOCKADDR_IN) + 16;

	// lpdwBytesReceived [out]
	// A pointer to a DWORD that receives the count of bytes received. 
	// This parameter is set only if the operation completes synchronously. 
	// If it returns ERROR_IO_PENDING and is completed later, 
	// then this DWORD is never set and you must obtain the number of bytes read from the completion notification mechanism.
	DWORD dwBytesReceived = 0;

	// lpOverlapped [in]
	// An OVERLAPPED structure that is used to process the request. This parameter must be specified; it cannot be NULL.
	//TODO 这里的PRE_IO_DATA应该改为对象池
	memset(&preIoData->m_BasePreIoData.m_OverLapped, 0x00, sizeof (preIoData->m_BasePreIoData.m_OverLapped));

	// 一些流行的Windows产品, 比如防病毒软件或虚拟专用网络软件, 会干扰AcceptEx()的正确操作
	// If no error occurs, the AcceptEx function completed successfully and a value of TRUE is returned.
	// If the function fails, AcceptEx returns FALSE. 
	// The WSAGetLastError function can then be called to return extended error information. 
	// If WSAGetLastError returns ERROR_IO_PENDING, then the operation was successfully initiated and is still in progress. 
	// If the error is WSAECONNRESET, an incoming connection was indicated, but was subsequently terminated by the remote peer prior to accepting the call.
	if ( !CWSAExtensionFunction::AcceptEx( pAcceptor->getHandle(), sAcceptSock, preIoData->m_szAcceptOutputBuffer, dwReceiveDataLength, 
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytesReceived, &preIoData->m_BasePreIoData.m_OverLapped ) )
	{
		nRetCode = GetLastNetError();
		if ( nRetCode != ERROR_IO_PENDING )
			return nRetCode;
	}

	preIoData.incRef();
	return 0;
}

#endif
