#ifndef _IOCP_DEF_H_
#define _IOCP_DEF_H_

#include <net/acceptor.h>
#include <net/connector.h>
#include <net/connection.h>

/**  
 * IoCompletionPort ʹ�õ��Ľṹ����
 */

// ��IO�������� (����)
typedef struct _tag_iocp_base_pre_io_data
{
	// �ص�IO�ṹ
	OVERLAPPED m_OverLapped;

	// ��������
	enum IOCP_OPERATOR_TYPE
	{
		IOCP_OPERATOR_ACCEPT,
		IOCP_OPERATOR_SEND,
		IOCP_OPERATOR_RECV,
		IOCP_OPERATOR_CONNECT
	}m_OperatorType;

}BASE_PRE_IO_DATA, *LPBASE_PRE_IO_DATA;

// Accept��IO��������
typedef struct _tag_accept_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// Accept���յ�Buff
	char m_szAcceptOutputBuffer [ ( sizeof(SOCKADDR_IN) + 16 ) + ( sizeof(SOCKADDR_IN) + 16 ) + 0 ];

	// Accept���ڽ��տͻ��˵�Socket
	SOCKET m_sAcceptSocket;

	// Acceptor
	CAcceptorPtr m_pAcceptor;

}ACCEPT_PRE_IO_DATA, *LPACCEPT_PRE_IO_DATA;

// Receive ��IO��������
typedef struct _tag_recv_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// dwFlags ��־λ
	DWORD m_dwFlags;

	// ������
	WSABUF m_Buffer;

	// ���ջ�����
	char szBuf[1024];

	// Connection
	CConnectionPtr m_pConnection;

}RECV_PRE_IO_DATA, *LPRECV_PRE_IO_DATA;

// Send ��IO��������
typedef struct _tag_send_pre_io_data
{
	//
	BASE_PRE_IO_DATA m_BasePreIoData;

	// dwNumberOfBytesSent
	DWORD m_dwNumberOfBytesSent;

	// ������
	WSABUF m_Buffer;

	// ���ͻ�����
	char szBuf[1024];

	// Connection
	CConnectionPtr m_pConnection;

}SEND_PRE_IO_DATA, *LPSEND_PRE_IO_DATA;

// Connect ��IO��������
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
