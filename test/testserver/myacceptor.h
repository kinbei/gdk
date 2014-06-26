#ifndef _MY_ACCEPTOR_H_
#define _MY_ACCEPTOR_H_

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
class CMyAcceptor : public CAcceptor
{
public:
	/**
	 * 
	 */
	CMyAcceptor()
	{
	}
	/**
	 * 
	 */
	virtual ~CMyAcceptor()
	{

	}
	
	/**
	 *
	 * \param 
	 * \return 
	 */
	int32 open( const std::string& strIP, uint16 nPort )
	{
		RETURN_IF_FAILED( CAcceptor::open( strIP, nPort ) );

		this->setListener( new TAcceptorListener<CMyAcceptor>( this, &CMyAcceptor::onAccept ) );
		return 0;
	}

protected:
	/**
	 * accept an new connection
	 *
	 * \param pConnection is the new connection, you can call pConnection->close() reject the connection, call pConnection->setListener() set the listener
	 * \return 
	 */
	virtual void onAccept( CConnectionPtr pConnection )
	{
		pConnection->setListener( new TConnectionListener<CMyAcceptor>( this, 
			&CMyAcceptor::onSendCompleted, 
			&CMyAcceptor::onRecvCompleted, 
			&CMyAcceptor::onClose ) ); 

		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress == NULL )
		{
			NORMAL_REPORT( "pAddress is null" );
			return ;
		}

		// NORMAL_REPORT( "new Connection: %s", pAddress->asString() );
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

		log_debug( "Connection(%p) recv buf %d bytes", pConnection.get(), pRecvBuf->getDataSize() );

		// need continue recv client data
		if( pRecvBuf->getDataSize() < g_nConfPreConnectionSendBytes )
			return ;

		// The client uses 0 - 255 character cycle fill
		while ( pRecvBuf->getDataSize() >= g_nConfPreConnectionSendBytes )
		{
			for ( int i = 0; i < g_nConfPreConnectionSendBytes; i++ )
			{
				int c = i % 255;

				if( pRecvBuf->getRowDataPointer()[i] != c )
					NORMAL_REPORT( "Invalid client data (%d) - (%d)", c, pRecvBuf->getRowDataPointer()[i] );
			}

			// echo the message
			pConnection->send( pRecvBuf->getRowDataPointer(), g_nConfPreConnectionSendBytes );

			//
			pRecvBuf->popBytes( g_nConfPreConnectionSendBytes );
		}
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
typedef TRefCountToObj<CMyAcceptor> CMyAcceptorPtr;

#endif
