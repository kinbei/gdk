#ifndef _IOCP_DEF_H_
#define _IOCP_DEF_H_

#include <net/acceptor.h>
#include <net/connector.h>
#include <net/connection.h>

/**  
 * IoCompletionPort 使用到的结构定义
 */

// 单IO操作数据 (基类)
typedef struct _tag_iocp_base_pre_io_data
{
	// 重叠IO结构
	OVERLAPPED m_OverLapped;

	// 操作类型
	enum IOCP_OPERATOR_TYPE
	{
		IOCP_OPERATOR_ACCEPT,
		IOCP_OPERATOR_SEND,
		IOCP_OPERATOR_RECV,
		IOCP_OPERATOR_CONNECT
	}m_OperatorType;

}BASE_PRE_IO_DATA, *LPBASE_PRE_IO_DATA;

// Accept单IO操作数据
typedef struct _tag_accept_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// Accept接收的Buff
	char m_szAcceptOutputBuffer [ ( sizeof(SOCKADDR_IN) + 16 ) + ( sizeof(SOCKADDR_IN) + 16 ) + 0 ];

	// Accept用于接收客户端的Socket
	SOCKET m_sAcceptSocket;

	// Acceptor
	CAcceptorPtr m_pAcceptor;

}ACCEPT_PRE_IO_DATA, *LPACCEPT_PRE_IO_DATA;

// Receive 单IO操作数据
typedef struct _tag_recv_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// dwFlags 标志位
	DWORD m_dwFlags;

	// 缓冲区
	WSABUF m_Buffer;

	// 接收缓冲区
	char szBuf[1024];

	// Connection
	CConnectionPtr m_pConnection;

}RECV_PRE_IO_DATA, *LPRECV_PRE_IO_DATA;

// Send 单IO操作数据
typedef struct _tag_send_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// dwNumberOfBytesSent
	DWORD m_dwNumberOfBytesSent;

	// 缓冲区
	WSABUF m_Buffer;

	// 发送缓冲区
	char szBuf[1024];

	// Connection
	CConnectionPtr m_pConnection;

}SEND_PRE_IO_DATA, *LPSEND_PRE_IO_DATA;

// Connect 单IO操作数据
typedef struct _tag_connect_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	//
	DWORD dwBytesSent;

	//
	CConnectorPtr m_pConnector;

}CONNECT_PRE_IO_DATA, *LPCONNECT_PRE_IO_DATA;

#endif
