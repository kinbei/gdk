#ifndef _EPOLL_NET_IO_WRAPPERS_H_
#define _EPOLL_NET_IO_WRAPPERS_H_

#include <vector>
#include <net/netpublic.h>
#include <net/acceptor.h>
#include <util/debug.h>
#include <net/netpublic.h>
#include <string>

#ifdef UNIX

#define MAX_FDS 1024

inline std::string getUnixErrorInfo( int nErrCode )
{
	char szBuf[1024] = {0};	
	snprintf( szBuf, sizeof(szBuf), "errcode(%d) errdesc(%s)", nErrCode, strerror( nErrCode ) );

	return szBuf;
}

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
		CEpollBindDataPtr pBindData = new CEpollBindData();
		pBindData->m_BindDataType = CEpollBindData::CONNECTOR_TYPE;
		pBindData->m_pConnector = pConnector;
		pBindData.incRef();

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = (void*)pBindData.getRefCountBase();

		log_debug( "addConnector|binddata(%p) ptr(%p)", pBindData.getRefCountBase(), ev.data.ptr );

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

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.ptr = (void*)pBindData.getRefCountBase();

		log_debug( "addAcceptor|binddata(%p) ptr(%p)", pBindData.getRefCountBase(), ev.data.ptr );

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
		struct epoll_event events[1024];

		while ( true )
		{
			int nfds = epoll_wait( m_nEpollFd, events, MAX_FDS, 1000 );

			int retcode = errno;

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
				log_debug("epollwait|binddata(%p)", events[i].data.ptr);

				CEpollBindDataPtr pBindData ( CRefCount( (CRefCountBase*) events[i].data.ptr ) );

				switch ( pBindData->m_BindDataType )
				{
				case CEpollBindData::CONNECTOR_TYPE:
					{
						CConnectorPtr pConnector = pBindData->m_pConnector;

						IConnectorListenerPtr pListener = pConnector->getListener();
						if ( pListener == NULL )
						{
							log_debug( "Connect|Connector(%p) Listener is null", pConnector.get());
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
							log_debug( "Accept|Acceptor(%p) Listener is null", pAcceptor.get());
							continue ;
						}

						struct sockaddr_in addr;
						memset(&addr, 0x00, sizeof(addr));
						socklen_t len = sizeof(addr);

						CTCPSocketPtr pRemoteSocket = pAcceptor->accept( (struct sockaddr*)&addr, &len );
						if ( pRemoteSocket == NULL )
						{
							log_warning( "Accept|Acceptor(%p) RemoteSocket is null(%s)", pAcceptor.get(), getUnixErrorInfo( GetLastNetError() ).c_str() );
							continue ;
						}

						CAddressPtr pAddress = new CAddress( &addr );
						CConnectionPtr pConnection = new CConnection( new CTCPSocket( pRemoteSocket->getHandle() ), pAddress );

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
							log_debug( "Connection(%p) event epollin", pConnection.get() );

							int32 nRetCode = pConnection->recv();

							// Upon successful completion, recv() shall return the length of the message in bytes. 
							// If no messages are available to be received and the peer has performed an orderly
							// shutdown, recv() shall return 0. Otherwise, -1 shall be returned and errno set to indicate the error.
							if ( nRetCode == -1 || nRetCode == 0 )
							{
								onConnectionClose( pConnection );
								pBindData.decRef();
								continue ;
							}

							IConnectionListenerPtr pListener = pConnection->getListener();
							if ( pListener == NULL )
							{
								log_debug( "Connection|Connection(%p) Listener is null", pConnection.get());
								continue ;
							}

							pListener->onRecvCompleted( pConnection );


							if ( pConnection->needSend() )
							{
								log_debug( "Connection(%p) add epollout", pConnection.get() );

								//
								events[i].events |= EPOLLOUT;

								if( modifyConnectionEvent( pConnection, &events[i] ) != 0 )
								{
									onConnectionClose( pConnection );
									pBindData.decRef();
									continue ;
								}
							}
						}
						else if ( events[i].events & EPOLLOUT )
						{
							log_debug( "Connection(%p) event epollout", pConnection.get() );

							int32 nRetCode = pConnection->send();

							if ( nRetCode != 0 )
								log_warning( "Connection|Connection(%p) Failed to send (%s)", pConnection.get(), getUnixErrorInfo( nRetCode ).c_str() );

							if ( !pConnection->needSend() )
							{
								log_debug( "Connection(%p) don't need send remove epollout", pConnection.get() );

								events[i].events = EPOLLIN | EPOLLHUP | EPOLLERR;

								if( modifyConnectionEvent( pConnection, &events[i] ) != 0 )
								{
									onConnectionClose( pConnection );
									pBindData.decRef();
									continue ;
								}
							}
						}
						else if ( events[i].events & EPOLLHUP || events[i].events & EPOLLERR )
						{
							log_debug( "Connection(%p) event epollhup or epollerr", pConnection.get() );

							onConnectionClose( pConnection );
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

	/**
	 * 
	 */
	virtual void getAllConnection( map<CConnector*, CConnectionPtr>& mapConnection )
	{
		mapConnection = m_mapConnection;
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

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
		ev.data.ptr = (void*)pBindData.getRefCountBase();

		log_debug("addConnection|binddata(%p) ptr(%p)", pBindData.getRefCountBase(), ev.data.ptr);

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_ADD, pConnection->getHandle(), &ev ) != 0 ) 
		{
			log_warning( "Connection|Connection(%p) Failed to add connection", pConnection.get());

			IConnectionListenerPtr pListener = pConnection->getListener();
			if ( pListener == NULL )
			{
				log_debug( "Connection|Connection(%p) Listener is null", pConnection.get());
				return GetLastNetError();
			}

			pListener->onClose( pConnection );
			return GetLastNetError();
		}

		m_mapConnection[ pConnection.get() ] = pConnection;
		return 0;
	}

	/**
	 * 
	 */
	virtual int32 delConnection( CConnectionPtr pConnection )
	{
		struct epoll_event ev;
		ev.events = 0;
		ev.data.ptr = (void*)NULL;

		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_DEL, pConnection->getHandle(), &ev ) != 0 ) 
		{
			log_warning( "Connection|Connection(%p) Failed to del connection", pConnection.get());
			return GetLastNetError();
		}

		return 0;
	}

	/**
	 * 
	 */
	int32 modifyConnectionEvent( CConnectionPtr pConnection, epoll_event* event )
	{
		// When successful, epoll_ctl returns zero. When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
		if ( epoll_ctl( m_nEpollFd, EPOLL_CTL_MOD, pConnection->getHandle(), event ) != 0 ) 
		{
			log_warning( "Connection|Connection(%p) Failed to modify connection event (%s)", pConnection.get(), getUnixErrorInfo( GetLastNetError() ).c_str() );
			return -1;
		}

		return 0;
	}

	/**
	 * 
	 */
	void onConnectionClose( CConnectionPtr pConnection )
	{
		//
		std::map< CConnection*, CConnectionPtr >::iterator iter_t;
		iter_t = m_mapConnection.find();

		assert( iter_t != m_mapConnection.end() );
		if ( iter_t != m_mapConnection.end() )
			m_mapConnection.erase( iter_t );

		// 
		IConnectionListenerPtr pListener = pConnection->getListener();
		if ( pListener == NULL )
		{
			log_debug( "Connection|Connection(%p) Listener is null", pConnection.get());
			return ;
		}

		pListener->onClose( pConnection );
		( void )( delConnection( pConnection ) );
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
	// 
	std::map< CConnection*, CConnectionPtr > m_mapConnection;
};
typedef TRefCountToObj<CEpollNetIoWrappers> CEpollNetIoWrappersPtr;

#endif // #ifdef UNIX

#endif
