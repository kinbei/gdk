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
	 * 初始化
	 *
	 * \return 返回0表示成功, 否则表示失败
	 */
	virtual int32 init()
	{
		//TODO epoll_create 的 size应该设置多少合适
		// When successful, epoll_create(2) returns a non-negative integer identifying the descriptor.  
		// When an error occurs, epoll_create(2) returns -1 and errno is set appro‐priately.
		m_nEpollFd = epoll_create(MAX_FDS);

		if ( m_nEpollFd == -1 )       
			return errno;

		return 0;
	}

	/**
	 * 反初始化, 程序结束时调用
	 */
	virtual void uninit()
	{
		//
		close( m_nEpollFd );
	}

	/**
	 * 注册 Connector 事件的处理
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
	 * 函数说明
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor )
	{

	}

	/**
	 * 网络事件处理, 程序可以一直调用此函数直至程序结束
	 *
	 * \param nTimeOutMilliseconds  超时的时间, 单位为毫秒
	 * \return 无
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
	 * 获取系统CPU个数
	 *
	 * \param 
	 * \return 
	 */
	uint32 getCPUCount();


	/**
	 * 工作线程
	 *
	 * \param 
	 * \return 
	 */
	static UINT WINAPI WorkerThread( LPVOID pParam );

	/**
	 * 投递Send
	 *
	 * \param 
	 * \return 
	 */
	int32 postSend( CConnectionPtr pConnection );

	/**
	 * 投递 Connect 请求
	 *
	 * \param 
	 * \return 
	 */
	int32 postConnect( CConnectorPtr pConnector );

	/**
	 * 投递Recv请求
	 *
	 * \param 
	 * \return 
	 */
	int32 postRecv( CConnectionPtr pConnection );

	/**
	 * 投递Accept请求
	 *
	 * \param 
	 * \return 
	 */
	int32 postAccept( CAcceptorPtr pAcceptor );

private:
	// epoll
	int m_nEpollFd;
	// Acceptor列表,  用于资源释放
	std::vector<CAcceptorPtr> m_vecAcceptor;
	// Connector列表, 用于资源释放
	std::vector<CConnectorPtr> m_vecConnector;
};
typedef TRefCountToObj<CEpollNetIoWrappers> CEpollNetIoWrappersPtr;

#endif // #ifdef UNIX

#endif
