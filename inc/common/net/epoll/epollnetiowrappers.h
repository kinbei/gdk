#ifndef _EPOLL_NET_IO_WRAPPERS_H_
#define _EPOLL_NET_IO_WRAPPERS_H_

#include <vector>
#include <net/netpublic.h>
#include <net/netiowrappers.h>
#include <net/acceptor.h>
#include <process.h>
#include <net/iocp/iocpdef.h>
#include <util/debug.h>

#ifdef UNIX

/**
 * epoll
 */
class CEpollNetIoWrappers : public INetIoWrappers
{
public:
	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	CEpollNetIoWrappers()
	{
		// epoll
		m_nEpollFd = 0;
	}

	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	virtual ~CEpollNetIoWrappers()
	{

	}

	/**
	 * ��ʼ��
	 *
	 * \return ����0��ʾ�ɹ�, �����ʾʧ��
	 */
	virtual int32 init()
	{
		//TODO epoll_create �� sizeӦ�����ö��ٺ���
		// When successful, epoll_create(2) returns a non-negative integer identifying the descriptor.  
		// When an error occurs, epoll_create(2) returns -1 and errno is set appro�\priately.
		m_nEpollFd = epoll_create(MAX_FDS);

		if ( m_nEpollFd == -1 )       
			return errno;

		return 0;
	}

	/**
	 * ����ʼ��, �������ʱ����
	 */
	virtual void uninit()
	{
		//
		close( m_nEpollFd );
	}

	/**
	 * ע�� Connector �¼��Ĵ���
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addConnector( CConnectorPtr pConnector )
	{
		// The struct epoll_event is defined as :
		// 	typedef union epoll_data {
		// 		void *ptr;
		// 		int fd;
		// 		__uint32_t u32;
		// 		__uint64_t u64;
		// 	} epoll_data_t;
		// 
		// 	struct epoll_event {
		// 		__uint32_t events;      /* Epoll events */
		// 		epoll_data_t data;      /* User data variable */
		// 	};

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = (void*)pConnector;

		pConnector->incRef();

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_ADD, pConnector->getSocket(), &ev ) != 0 ) 
			return GetLastNetError();

		return SUCCESS;
	}

	/**
	 * ����˵��
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor )
	{

	}

	/**
	 * �����¼�����, �������һֱ���ô˺���ֱ���������
	 *
	 * \param nTimeOutMilliseconds  ��ʱ��ʱ��, ��λΪ����
	 * \return ��
	 */
	virtual int32 run( int32 nTimeOutMilliseconds );

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop();

protected:
	/**
	 * ��ȡϵͳCPU����
	 *
	 * \param 
	 * \return 
	 */
	uint32 getCPUCount();


	/**
	 * �����߳�
	 *
	 * \param 
	 * \return 
	 */
	static UINT WINAPI WorkerThread( LPVOID pParam );

	/**
	 * Ͷ��Send
	 *
	 * \param 
	 * \return 
	 */
	int32 postSend( CConnectionPtr pConnection );

	/**
	 * Ͷ�� Connect ����
	 *
	 * \param 
	 * \return 
	 */
	int32 postConnect( CConnectorPtr pConnector );

	/**
	 * Ͷ��Recv����
	 *
	 * \param 
	 * \return 
	 */
	int32 postRecv( CConnectionPtr pConnection );

	/**
	 * Ͷ��Accept����
	 *
	 * \param 
	 * \return 
	 */
	int32 postAccept( CAcceptorPtr pAcceptor );

private:
	// epoll
	int m_nEpollFd;
	// Acceptor�б�,  ������Դ�ͷ�
	std::vector<CAcceptorPtr> m_vecAcceptor;
	// Connector�б�, ������Դ�ͷ�
	std::vector<CConnectorPtr> m_vecConnector;
};
typedef TRefCountToObj<CEpollNetIoWrappers> CEpollNetIoWrappersPtr;

#endif // #ifdef UNIX

#endif
