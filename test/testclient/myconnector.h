#ifndef _MY_CONNECTOR_H_
#define _MY_CONNECTOR_H_

#include <net/acceptor.h>
#include <util/debug.h>

#define ERROR_REPORT(fmt, ...) { fprintf( stderr, "> " fmt "\n", ##__VA_ARGS__); fflush(stderr); }
#define NORMAL_REPORT(fmt, ...) { fprintf( stdout, "> " fmt "\n", ##__VA_ARGS__); fflush(stdout); }

#define RETURN_IF_FAILED( x ) \
	do \
	{ \
		int32 nRet = ( x ); \
		if ( nRet != 0 ) \
			return nRet; \
	} while ( false ); \

extern long g_nConfPreConnectionSendBytes;

/**
 * 
 */
class CMyConnector : public CConnector
{
public:
	/**
	 * 
	 */
	CMyConnector()
	{
	}
	/**
	 * 
	 */
	virtual ~CMyConnector()
	{

	}
	
	/**
	 *
	 * \param 
	 * \return 
	 */
	int32 open( const std::string& strIP, uint16 nPort )
	{
		RETURN_IF_FAILED( CConnector::connect( strIP.c_str(), nPort ) );

		this->setListener( new TConnectorListener<CMyConnector>( this, &CMyConnector::onOpen ) );
		return 0;
	}

protected:
	/**
	 * accept an new connection
	 *
	 * \param pConnection is the new connection, you can call pConnection->close() reject the connection, call pConnection->setListener() set the listener
	 * \return 
	 */
	virtual void onOpen( CConnectionPtr pConnection )
	{
		pConnection->setListener( new TConnectionListener<CMyConnector>( this, 
			&CMyConnector::onSendCompleted, 
			&CMyConnector::onRecvCompleted, 
			&CMyConnector::onClose ) ); 

		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress == NULL )
		{
			NORMAL_REPORT( "pAddress is null" );
			return ;
		}

		// NORMAL_REPORT( "success to connect server remote address : %s", pAddress->asString() );

		// send message to server
		char* pBuf = new char[ g_nConfPreConnectionSendBytes ];
		for ( int i = 0; i < g_nConfPreConnectionSendBytes; i++ )
			pBuf[i] = i % 255;
		
		pConnection->send( pBuf, g_nConfPreConnectionSendBytes );
		delete[] pBuf;
	}

	/**
	 * onRecv
	 *
	 * \param 
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection )
	{
		CBytesBufferPtr pRecvBuf = pConnection->getRecvBuffer();
		if ( pRecvBuf == NULL )
		{
			NORMAL_REPORT( "recv buffer is null" );
			return ;
		}
		
		// echo the message
		pConnection->send( pRecvBuf->getRowDataPointer(), pRecvBuf->getDataSize() );

		// need continue recv client data?
		if( pRecvBuf->getDataSize() < g_nConfPreConnectionSendBytes )
			return ;

		int nInvalidBytes = 0;

		// The client uses 0 - 255 character cycle fill
		while ( pRecvBuf->getDataSize() >= g_nConfPreConnectionSendBytes )
		{
			for ( int i = 0; i < g_nConfPreConnectionSendBytes; i++ )
			{
				// must use char type, otherwide, it will failed to check sign data
				char c = i % 255;

				if( pRecvBuf->getRowDataPointer()[i] != c )
				{
					//	NORMAL_REPORT( "Invalid client data (%d) - (%d)", c, pRecvBuf->getRowDataPointer()[c] );
					nInvalidBytes++;
				}
			}

			pRecvBuf->popBytes( g_nConfPreConnectionSendBytes );
		}

		// print the count of invalid bytes
		if ( nInvalidBytes != 0 )
			NORMAL_REPORT( "Server Invalid server bytes (%d)", nInvalidBytes );
	}

	/**
	 * onSend
	 *
	 * \param 
	 * \return 
	 */
	virtual void onSendCompleted( CConnectionPtr pConnection, uint32 nSendBytes )
	{

	}

	/**
	 * onClose
	 *
	 * \param 
	 * \return 
	 */
	virtual void onClose( CConnectionPtr pConnection )
	{
		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress != NULL )
			NORMAL_REPORT( "Connection(%s) closed", pAddress->asString() );
	}

private:

};
typedef TRefCountToObj<CMyConnector> CMyConnectorPtr;

#endif
