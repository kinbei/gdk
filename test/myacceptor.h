#ifndef _MY_ACCEPTOR_H_
#define _MY_ACCEPTOR_H_

#include <net/acceptor.h>
#include <util/debug.h>

#define RETURN_IF_FAILED( x ) \
	do \
	{ \
		int32 nRet = ( x ); \
		if ( nRet != 0 ) \
			return nRet; \
	} while ( false ); \

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
	 * 接受新的连接处理
	 *
	 * \param pConnection 连上来的新连接, 可以通过 pConnection->close() 拒绝连接, pConnection->setListener() 设置回调函数
	 * \return 
	 */
	virtual void onAccept( CConnectionPtr pConnection )
	{
		// 指定连接的网络处理事件
		pConnection->setListener( new TConnectionListener<CMyAcceptor>( this, 
			&CMyAcceptor::onSendCompleted, 
			&CMyAcceptor::onRecvCompleted, 
			&CMyAcceptor::onClose ) ); 

		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress == NULL )
		{
			DEBUG_INFO( "pAddress is null" );
			return ;
		}

		DEBUG_INFO( "new Connection: %s", pAddress->asString() );
	}

	/**
	 * onRecv
	 *
	 * \param 
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection, const char *pBuf, uint32 nRecvLen )
	{
		// pConnection->recv( pBuf, nRecvLen );

		char *pszBuf = new char[nRecvLen + 1];
		memset(pszBuf, 0x00, nRecvLen + 1);
		memcpy( pszBuf, pBuf, nRecvLen );
		DEBUG_INFO( pszBuf );
		delete[] pszBuf;
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
			DEBUG_INFO( "Connection(%s) closed", pAddress->asString() );
	}

private:

};
typedef TRefCountToObj<CMyAcceptor> CMyAcceptorPtr;

#endif
