#ifndef _EPOLL_NET_IO_WRAPPERS_H_
#define _EPOLL_NET_IO_WRAPPERS_H_

#include <vector>
#include <net/netpublic.h>
#include <net/acceptor.h>
#include <util/debug.h>
#include <net/netpublic.h>

#ifdef UNIX

#define MAX_FDS 1024

class CEpollBindData
{
public:
	CEpollBindData()
	{
		//
		m_pConnector = NULL;
		//
		m_pAcceptor = NULL;
		//
		m_pConnection = NULL;
	}

	enum
	{
		CONNECTOR_TYPE = 1,
		ACCEPTOR_TYPE = 2,
		CONNECTION_TYPE = 3
	} m_BindDataType;

	//
	CConnectorPtr m_pConnector;
	//
	CAcceptorPtr m_pAcceptor;
	//
	CConnectionPtr m_pConnection;
};
typedef TRefCountToObj<CEpollBindData> CEpollBindDataPtr;

/**
 * epoll
 */
class CEpollNetIoWrappers : public INetIoWrappers
{
public:
	/**
	 *
	 */
	CEpollNetIoWrappers()
	{
		// epoll
		m_nEpollFd = 0;
	}

	/**
	 *
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
		//TODO pBindData ��ʱ decRef() ?

		CEpollBindDataPtr pBindData = new CEpollBindData();
		pBindData->m_BindDataType = CEpollBindData::CONNECTOR_TYPE;
		pBindData->m_pConnector = pConnector;
		pBindData.incRef();

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = (void*)pBindData.get();

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_ADD, pConnector->getHandle(), &ev ) != 0 ) 
			return GetLastNetError();

		return 0;
	}

	/**
	 * ����˵��
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor )
	{
		CEpollBindDataPtr pBindData = new CEpollBindData();
		pBindData->m_BindDataType = CEpollBindData::ACCEPTOR_TYPE;
		pBindData->m_pAcceptor = pAcceptor;
		pBindData.incRef();

		//TODO pBindData ��ʱ decRef() ?

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = (void*)pBindData.get();

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_ADD, pAcceptor->getHandle(), &ev ) != 0 ) 
			return GetLastNetError();

		return 0;
	}

	/**
	 * �����¼�����, �������һֱ���ô˺���ֱ���������
	 *
	 * \param nTimeOutMilliseconds  ��ʱ��ʱ��, ��λΪ����
	 * \return ��
	 */
	virtual int32 run( int32 nTimeOutMilliseconds = 100 )
	{
		struct epoll_event events[100];

		while ( true )
		{
			int nfds = epoll_wait( m_nEpollFd, events, MAX_FDS, 1000 );

			// When an error occurs, epoll_wait returns -1 and errno is set appropriately.
			if ( nfds == -1 )
			{
				if( errno == EINTR )
					continue;

				stop();
				return errno;
			}

			for( int i = 0; i < nfds; i++ )
			{
				CEpollBindDataPtr pBindData = (CEpollBindData*)( events[i].data.ptr );

				switch ( pBindData->m_BindDataType )
				{
				case CEpollBindData::CONNECTOR_TYPE:
					{
						CConnectorPtr pConnector = pBindData->m_pConnector;

						IConnectorListenerPtr pListener = pConnector->getListener();
						if ( pListener == NULL )
						{
							log_debug( "Connect|Connector(0x%08X) Listener is null", pConnector.get());
							continue ;
						}

						CConnectionPtr pConnection = new CConnection( new CTCPSocket( pConnector->getHandle() ), pConnector->getAddress() );

						if( addConnection( pConnection ) != 0 )
							continue ;

						pListener->onOpen( pConnection );				
					}
					break;
				
				case CEpollBindData::ACCEPTOR_TYPE:
					{
						CAcceptorPtr pAcceptor = pBindData->m_pAcceptor;

						IAcceptorListenerPtr pListener = pAcceptor->getListener();
						if ( pListener == NULL )
						{
							log_debug( "Accept|Acceptor(0x%08X) Listener is null", pAcceptor.get());
							continue ;
						}

						struct sockaddr_in addr;
						memset(&addr, 0x00, sizeof(addr));
						socklen_t len = sizeof(struct sockaddr);

						CTCPSocketPtr pRemoteSocket = pAcceptor->accept( (struct sockaddr*)&addr, &len );
						if ( pRemoteSocket == NULL )
						{
							log_warning( "Accept|Acceptor(0x%08X) RemoteSocket is null", pAcceptor.get());
							continue ;
						}

						CAddressPtr pAddress = new CAddress( &addr );

						CConnectionPtr pConnection = new CConnection( new CTCPSocket( pRemoteSocket->getHandle() ), pAddress.get() );

						if( addConnection( pConnection ) != 0 )
							continue ;

						pListener->onAccept( pConnection );
					}
					break;
				
				case CEpollBindData::CONNECTION_TYPE:
					{
						CConnectionPtr pConnection = pBindData->m_pConnection;

						if ( events[i].events & EPOLLIN )
						{
							// recvive data

							int32 nRetCode = pConnection->recv();

							// Upon successful completion, recv() shall return the length of the message in bytes. 
							// If no messages are available to be received and the peer has performed an orderly
							// shutdown, recv() shall return 0. Otherwise, -1 shall be returned and errno set to indicate the error.
							if ( nRetCode == -1 )
							{
								log_warning( "Connection|Connection(0x%08X) Failed to recv Error(%d)", pConnection.get(), GetLastNetError());
								continue ;
							}

							if ( nRetCode == 0 )
							{
								IConnectionListenerPtr pListener = pConnection->getListener();
								if ( pListener == NULL )
								{
									log_debug( "Connection|Connection(0x%08X) Listener is null", pConnection.get());
									continue ;
								}

								pListener->onClose( pConnection );
								( void )( delConnection( pConnection ) );
								pBindData.decRef();
								continue ;
							}

							IConnectionListenerPtr pListener = pConnection->getListener();
							if ( pListener == NULL )
							{
								log_debug( "Connection|Connection(0x%08X) Listener is null", pConnection.get());
								continue ;
							}

							pListener->onRecvCompleted( pConnection );
						}
						else if ( events[i].events & EPOLLOUT )
						{
							int32 nRetCode = pConnection->send();
							if ( nRetCode != 0 )
								log_warning( "Connection|Connection(0x%08X) Failed to send Error(%d)", pConnection.get(), nRetCode);

							if ( nRetCode != EAGAIN )
								continue ;

							if ( !pConnection->needSend() )
							{
								events[i].events &= ~EPOLLOUT;
								
								// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
								if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_MOD, pConnection->getHandle(), &events[i] ) != 0 ) 
								{
									log_warning( "Connection|Connection(0x%08X) Failed to modify connection", pConnection.get());

									IConnectionListenerPtr pListener = pConnection->getListener();
									if ( pListener == NULL )
									{
										log_debug( "Connection|Connection(0x%08X) Listener is null", pConnection.get());
										continue ;
									}

									pListener->onClose( pConnection );
									( void )( delConnection( pConnection ) );
									pBindData.decRef();
									continue ;
								}
							}
						}
						else if ( events[i].events & EPOLLHUP || events[i].events & EPOLLERR )
						{
							IConnectionListenerPtr pListener = pConnection->getListener();
							if ( pListener == NULL )
							{
								log_debug( "Connection|Connection(0x%08X) Listener is null", pConnection.get());
								continue ;
							}

							pListener->onClose( pConnection );
							( void )( delConnection( pConnection ) );
							pBindData.decRef();
							continue ;
						}
					}
					break;
				}
			} // for

		} // while

		return 0;
	}

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop()
	{

	}

protected:
	/**
	 * 
	 */
	virtual int32 addConnection( CConnectionPtr pConnection )
	{
		CEpollBindDataPtr pBindData = new CEpollBindData();
		pBindData->m_BindDataType = CEpollBindData::CONNECTION_TYPE;
		pBindData->m_pConnection = pConnection;
		pBindData.incRef();

		//TODO pBindData ��ʱ decRef() ?

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
		ev.data.ptr = (void*)pBindData.get();

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_ADD, pConnection->getHandle(), &ev ) != 0 ) 
		{
			log_warning( "Connection|Connection(0x%08X) Failed to add connection", pConnection.get());

			IConnectionListenerPtr pListener = pConnection->getListener();
			if ( pListener == NULL )
			{
				log_debug( "Connection|Connection(0x%08X) Listener is null", pConnection.get());
				return GetLastNetError();
			}

			pListener->onClose( pConnection );
			return GetLastNetError();
		}

		return 0;
	}

	/**
	 * 
	 */
	virtual int32 delConnection( CConnectionPtr pConnection )
	{
		CEpollBindDataPtr pBindData = new CEpollBindData();
		pBindData->m_BindDataType = CEpollBindData::CONNECTION_TYPE;
		pBindData->m_pConnection = pConnection;
		pBindData.incRef();

		//TODO pBindData ��ʱ decRef() ?

		struct epoll_event ev;
		ev.events = 0;
		ev.data.ptr = (void*)pBindData.get();

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_DEL, pConnection->getHandle(), &ev ) != 0 ) 
		{
			log_warning( "Connection|Connection(0x%08X) Failed to del connection", pConnection.get());
			return GetLastNetError();
		}

		return 0;
	}

// 	/**
// 	 * �����߳�
// 	 *
// 	 * \param 
// 	 * \return 
// 	 */
// 	static UINT WINAPI WorkerThread( LPVOID pParam );

// 	/**
// 	 * Ͷ��Send
// 	 *
// 	 * \param 
// 	 * \return 
// 	 */
// 	int32 postSend( CConnectionPtr pConnection );
// 
// 	/**
// 	 * Ͷ�� Connect ����
// 	 *
// 	 * \param 
// 	 * \return 
// 	 */
// 	int32 postConnect( CConnectorPtr pConnector );
// 
// 	/**
// 	 * Ͷ��Recv����
// 	 *
// 	 * \param 
// 	 * \return 
// 	 */
// 	int32 postRecv( CConnectionPtr pConnection );
// 
// 	/**
// 	 * Ͷ��Accept����
// 	 *
// 	 * \param 
// 	 * \return 
// 	 */
// 	int32 postAccept( CAcceptorPtr pAcceptor );

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
